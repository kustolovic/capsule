<?php
namespace capsule\Commands;

class Module extends \capsule\Command {
  public function argsDefinition(){
    return array(
      'enable/$' => array(),
      'list' => array(),
    );
  }

  public function output(){

      fwrite(STDOUT, "Unknown module arguments\n");
  }
}