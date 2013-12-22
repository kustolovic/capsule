<?php
namespace Commands;

class Help extends Command {
  
  public function output(){
    fwrite(STDOUT, "@TODO: write some fuckin help!\n");
  }

}