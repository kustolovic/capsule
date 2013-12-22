<?php

function capsule_get_template($page, $variables=array()){
  // Extract the variables to a local namespace
  extract($variables, EXTR_SKIP);

  // Start output buffering
  ob_start();

  // Include the template file
  include ROOT . '/' . $page;

  // End buffering and return its contents
  return ob_get_clean();
}

function capsule_render_html_page(){
  $variables['ln'] = 'fr';
  $variables['pagename'] = 'test';
  $variables['menu'] = array();

  foreach ($pages as $name => $values) {
    $values['all'] = capsule_get_default_menu_item($values['all']);
    if ($values['all']['type'] == 'normal') {
      $variables['menu'][$name] = array(
        'link' => '/' . $ln . '/' . $name,
        'text' => $values[$ln]['title']
      );
    }
  }
  $content = capsule_get_template($ln . '/' . $pages[$pageName]['file'], $variables);

  $variables['content'] = $content;
  $variables['errors'] = get_errors();
  if (!isset($variables['pagetitle'])) $variables['pagetitle'] = $variables['title'];

  $page = capsule_get_template('includes/page.php', $variables);
}