<?php
namespace capsule\Commands;

class Help extends \capsule\Command {
  
  public function output(){
    fwrite(STDOUT, "@TODO: write some fuckin help!\n");
  }

}