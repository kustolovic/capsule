<?php
use capsule\Cache;

/**
 * Various functions loaded in all capsule calls.
 */

/**
 * Get and parse a YAML file config, the parsed file is stored for further calls
 * @param String $file A path to the config file from the Root '/conf/router.yml'
 * @return array
 */
function capsule_get_conf_file($file){
  static $files = array();
  if (isset($files[$file])) return $files[$file];
  
  $contents = Spyc::YAMLLoad(ROOT . $file);
  $files[$file] = $contents;
  
  return $contents;
}

/**
 * get the default settings if not SET.
 * @return array
 */
function _capsule_get_default_settings(){
  return array(
    'multilang'=> FALSE,
    'language' => array(
      'default' => 'fr',
      'resolver' => array(
        'callback' => 'capsule_resolve_lang',
        'path'=> NULL
      )
    )
  );
}

/**
 * Get the settings file and merge with the defaults
 * @return array
 */
function capsule_get_settings(){
  static $settings = NULL;
  if (isset($settings)) return $settings;
  
  $settings = Spyc::YAMLLoad(ROOT . '/conf/settings.yml');
  
  $default = _capsule_get_default_settings();
  $settings = array_merge_recursive_distinct($settings, $default);
  
  return $settings;
}

/**
 * Merge the route settings with the defaults settings
 * @param array $variables route definition
 * @return array
 */
function capsule_get_default_menu_item($route){
  return array_merge(array(
    'type' => 'normal',
    'files' => array(),
    'page_init' => NULL,
    'preprocess_page' => array(),
    'preprocess_content' => array(),
    'dependencies' => array(),
    'authenticate' => NULL,
    'page_template' => 'content/system/page.inc'
  ), $route);
}

/**
 * Set a new error in session.
 * @param String $message text message
 * @param String $type error type, default to error
 * @param String $name error name, if needed
 */
function set_error($message, $type = 'error', $name = NULL){
  if (!$name) $name = 'e'.count($_SESSION['errors']);
  $_SESSION['errors'][$name] = array(
    'type' => $type,
    'message' => $message,
    'name' => $name
  );
}
/**
 * Get the errors message and empty the messages cache.
 * @return array errors
 */
function get_errors(){
  if (!isset($_SESSION['errors'])) return array();
  $errors = $_SESSION['errors'];
  $_SESSION['errors'] = array();
  return $errors;
}

/**
 * Redirects to a valid path and exit capsule processing
 * Use a header, so nothing can have been sent before
 * @param String $path like 'contact', 'foo/bar'
 */
function capsule_redirect($path) {
  $url = 'http://' .$_SERVER['HTTP_HOST'] . '/' . $path;
  header('Location: ' . $url, TRUE, 302);
  exit();
}

/**
 * Merge recursive
 * @param array $array1
 * @param array $array2
 * @return array
 */
function array_merge_recursive_distinct ( array &$array1, array &$array2 ) {
  $merged = $array1;

  foreach ( $array2 as $key => &$value ) {
    if ( is_array ( $value ) && isset ( $merged [$key] ) && is_array ( $merged [$key] ) ){
      $merged [$key] = array_merge_recursive_distinct ( $merged [$key], $value );
    }
    else{
      $merged [$key] = $value;
    }
  }

  return $merged;
}

/**
 * resolve the router
 * @param type $routes
 * @return array
 */
function capsule_router_resolver($routes){
  $resolver = array();
  foreach ($routes as $route => $value) {
    $parts = explode('/', $route);
    $resolver_recursive = &$resolver;
    foreach ($parts as $part) {
      $resolver_recursive = &$resolver_recursive[$part];
    }
    $resolver_recursive['#route'] = $route;
  }
  return $resolver;
}

/**
 * 
 * @param type $name
 * @param type $callback
 * @param type $duration
 * @return type
 */
function capsule_cache_get($name, $callback, $duration=0){
  $cache = new Cache($name, $duration);
  $result = $cache->read();
  if(!$result) {
  	$result = $callback();
  	$cache->write($result);
  }
  return $result;
}
/**
 * 
 * @param type $query
 * @return type
 */
function capsule_set_query($query){
  return capsule_get_query($query);
}
/**
 * 
 * @staticvar type $query
 * @param type $q
 * @return string
 */
function capsule_get_query($q = FALSE){
  static $query;
  if ($q) $query = $q;
  if ($query) return $query;
  $query = explode('?', $_SERVER['REQUEST_URI']);
  $query = explode('/',$query[0]);
  array_shift($query);
  if (!isset($query[0]) || !$query[0]) {
    $query[0] = 'home';
  }
  
  return $query;
}

/**
 * 
 * @param type $language
 * @return type
 */
function capsule_set_language($language){
  return capsule_get_language($language);
}

/**
 * 
 * @staticvar type $language
 * @param type $ln
 * @return type
 */
function capsule_get_language($ln = FALSE){
  static $language;
  if ($ln) $language = $ln;
  if ($language) return $language();
  
  $settings = capsule_get_settings();
  $path = capsule_get_query();
  
  
}

/**
 * get a template 
 * @param String $template_file path from ROOT like 'content/mypage.inc'
 * @param associative array $variables variables availables in the template
 * @return String
 */
function capsule_render_template($template_file, $variables) {
  extract($variables, EXTR_SKIP);
  ob_start();
  include ROOT . '/' . $template_file; 
  return ob_get_clean();
}

/**
 * Get the menu
 * @return type
 */
function capsule_get_default_menu(){
  return capsule_set_default_menu();
}

/**
 * 
 * @staticvar array $menu
 * @param type $resolver
 * @return array|string
 */
function capsule_set_default_menu($resolver = NULL){
  static $menu = array();
  if ($menu) return $menu;
  
  $routes = capsule_get_conf_file('/conf/router.yml');
  foreach ($resolver as $menuentry) {
    $menuitem = capsule_get_default_menu_item($routes[$menuentry['#route']]);
    
    if ($menuitem['type'] === 'normal') {
      $menu[$menuentry['#route']] = array(
        'link' => '/' . $menuentry['#route'],
        'text' => $menuitem['title']
      );
    }
  }
  return $menu;
  
}