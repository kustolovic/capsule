<?php
namespace Commands;

class Module extends Command {
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