<?php

// ––––––––––––––––––––––––––––––––––––––––––– MENU
function bootstrap_init($cli = FALSE){
  bootstrap_autoload();
  if (!$cli) session_start();
  bootstrap_include();
  if ($cli) return TRUE;
  $route = bootstrap_menu();
  $menu = bootstrap_default_menu();
  bootstrap_validate_access($route);
  bootstrap_load_dependencies($route);
  $page = bootstrap_get_page($route, $menu);
  return $page;
}

function bootstrap_include(){
  require ROOT . '/vendor/capsule/common.inc';
  require ROOT . '/vendor/capsule/page.inc';
}
function bootstrap_menu(){
  $settings = capsule_get_settings();
  
  $path = capsule_get_query();
  
  $resolver = capsule_cache_get('capsule_resolver',function(){
    $routes = capsule_get_conf_file('/conf/router.yml');
    return capsule_router_resolver($routes);
  });
  
  capsule_set_default_menu($resolver);
  
  $resolver_recursive = &$resolver;
  $route = NULL;
  $args = array();

  foreach ($path as $element) {
    if (isset($resolver_recursive[$element])) {
      $resolver_recursive = &$resolver_recursive[$element];
    }
    else if (isset ($resolver_recursive['%'])){
      $resolver_recursive = &$resolver_recursive['%'];
      $args[] = $element;
    }
    else {
      $args[] = $element;
    }
    if (isset($resolver_recursive['#route'])) $route = $resolver_recursive['#route'];
  }

  if (!isset($resolver[$route])){
    header("HTTP/1.0 404 Not Found");
    include ROOT . '/404.html';
    die();
  }
  else {
    $routes = capsule_get_conf_file('/conf/router.yml');
    $routes[$route]['#route'] = $route;
    return capsule_get_default_menu_item($routes[$route]);
  }
}
function bootstrap_default_menu(){
  return capsule_get_default_menu();
}
function bootstrap_load_dependencies($route){
  foreach ($route['dependencies'] as $dependency) {
    if (file_exists(ROOT . '/vendor/capsule/' . $dependency . '.inc')) {
      include_once ROOT . '/vendor/capsule/' . $dependency . '.inc';
    }
  }
  foreach ($route['files'] as $file) {
    if (file_exists(ROOT . '/' . $file)) {
      include_once ROOT . '/' . $file;
    }
  }
}
function bootstrap_validate_access($route){
  
  if ($realm = $route['authenticate']){
    $access = TRUE;
    $settings = capsule_get_conf_file('/conf/settings.yml');
    
    if (isset($_SERVER['PHP_AUTH_USER'])){
//      var_dump(md5($_SERVER['PHP_AUTH_PW']));
//      var_dump(isset($settings['authenticate'][$realm][$_SERVER['PHP_AUTH_USER']]));
//      var_dump($settings['authenticate'][$realm][$_SERVER['PHP_AUTH_USER']]);
    }
    if (!isset($settings['authenticate'][$realm])){
      $access = FALSE;
    }
    else {
      if (!isset($_SERVER['PHP_AUTH_USER'])
        || !isset($settings['authenticate'][$realm][$_SERVER['PHP_AUTH_USER']])
        || md5($_SERVER['PHP_AUTH_PW']) != $settings['authenticate'][$realm][$_SERVER['PHP_AUTH_USER']]) {
          $access = FALSE;
      }
    }
    if (!$access){
      header('WWW-Authenticate: Basic realm="Acceder a l\'administration"');
      header('HTTP/1.0 401 Unauthorized');
      echo 'Vous n’avez pas accès à cette page!';
      exit;
    }
  }
}
function bootstrap_get_page($route, $menu){
  $variables = array();
  
  
  if ($route['page_init']){
    if (function_exists($route['page_init'])){
      $route['page_init']($variables);
    }
  }
  $variables['errors'] = get_errors();
  
  $variables['pagename'] = $route['#route'];
  
  if ($route['preprocess_content']){
    
    foreach ($route['preprocess_content'] as $preprocess) {
      if (function_exists($preprocess)){
        $preprocess($variables);
      }
    }
  }
  $content = capsule_render_template('content/' . $route['file'], $variables);

  $variables['content'] = $content;
  $variables['menu'] = $menu;
  $variables['ln'] = 'fr';
  
  if (!isset($route['pagetitle'])) {
    $variables['pagetitle'] = $route['title'];
  }
  else {
    $variables['pagetitle'] = $route['pagetitle'];
  }
  
  if ($route['preprocess_page']){
    foreach ($route['preprocess_page'] as $preprocess) {
      if (function_exists($preprocess)){
        $preprocess($variables);
      }
    }
  }
  $page = capsule_render_template($route['page_template'], $variables);
  return $page;
}

function bootstrap_autoload(){
  require ROOT . '/vendor/autoload.php';
  spl_autoload_register(function($name){
    $parts = explode('\\', $name);
    if (count($parts) == 1){
      include ROOT . '/class/' . $name . '.php';
    }
    else {
      $path = '/class/';
      $class = FALSE;
      switch ($parts[0]) {
        case 'Commands':
          if ($parts[1] != 'Command') {
            $path = '/vendor/capsule/commands/';
          }
          $class = $parts[1];
          break;
        default :
          $path = '/vendor/'.$parts[0].'/class/';
          $class = $parts[1];
          for ($i = 2; isset($parts[$i]); $i++){
            $path .= $parts[$i-1].'/';
            $class = $parts[$i];
          }
          break;
      }
      if($class){
        include ROOT . $path . $class . '.php';
      }
    }
  });
  
}