<?php
namespace Commands;

class Cache extends \capsule\Command {

  public function output(){
    if (isset($this->args[0]) && $this->args[0] == 'clear') {
      $count = \capsule\Cache::clear();
      fwrite(STDOUT, "Removed $count files\n");
    }
    else {
      fwrite(STDOUT, "Unknown cache arguments\n");
    }
  }
}