<?php
namespace Commands;

class Command {
  protected $args;
  
  function __construct($args) {
    $this->args = $args;
  }
  
  public function output(){
    fwrite(STDOUT, "It works!\n");
  }

}