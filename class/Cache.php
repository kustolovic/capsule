<?php
namespace capsule;

class Cache {
  private $filename;
  private $duration;
  const CACHE_FOLDER = '/cache/';
  
  function __construct($name, $duration = 0) {
    $this->duration = $duration;
    $this->filename = $this->getFilename($name);
  }
  
  private function getFilename($name){
    return ROOT . self::CACHE_FOLDER . $name . '.cache';
  }
  
  private function open(){
    if (!file_exists($this->filename)) {
      return FALSE;
    }
    $file = file_get_contents($this->filename);
    return unserialize($file);
    
  }
  public function read() {
    if ( ($contents = $this->open()) === FALSE) return FALSE;
    if ($contents['cache_limit'] > 0 && $contents['cache_limit'] < time()) {
      unlink($this->filename);
      return FALSE;
    }
    return $contents['contents'];
  }
  
  /**
   * 
   * @param mixed $contents
   * @return returns the number of bytes written, or FALSE on error. 
   */
  public function write($contents) {
    $cache_limit = 0;
    if ($this->duration > 0){
      $cache_limit = time() + $this->duration;
    } 
    
    $f = fopen($this->filename, 'w');
    $res = fwrite($f, serialize(array(
      'cache_limit' => $cache_limit,
      'contents' => $contents
    )));
    fclose($f);
    
    return $res;
  }
  
  public static function clear(){
    $cacheD = ROOT . self::CACHE_FOLDER;
    $d = dir($cacheD);
    $i = 0;
    while (false !== ($entry = $d->read())) {
      if(!is_dir($cacheD.$entry) && $entry[0]!='.'){
        $i++;
        unlink($cacheD.$entry)."\n";
      }
    }
    $d->close();
    return $i;
  }
  public static function clean(){
    
  }
}
