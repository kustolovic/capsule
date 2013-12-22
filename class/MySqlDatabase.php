<?php

/**
 *  MySQL Database
 *
 *  A singleton object which provides convenience methods for interfacing with
 *  a MySQL database in PHP 5. You can get the object's instance using the 
 *  static {@link getInstance()} method. Being a singleton object, this class
 *  only supports one open database connection at a time and idealy suited to
 *  single-threaded applications. You can read 
 */
class MySqlDatabase extends PDO {

  protected static $instance;

  public static function getInstance() {

    if (empty(self::$instance)) {
      $settings = capsule_get_settings();
      $db_info = $settings['database'];
      self::$instance = new MySqlDatabase("mysql:host=" . $db_info['host'] . ';port=' . $db_info['port'] . ';dbname=' . $db_info['name'], $db_info['user'], $db_info['pass'], array(PDO::MYSQL_ATTR_INIT_COMMAND => 'SET NAMES \'UTF8\''));
    }

    return self::$instance;
  }

}