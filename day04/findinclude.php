<?php

function sign($n) {
    return ($n > 0) - ($n < 0);
}

$stdin = fopen('php://stdin','r');
while( $line = fgets( $stdin ) ) {
  if(!preg_match('/^(\d+)-(\d+),(\d+)-(\d+)$/', $line, $matches))
    continue;
  $diff1=intval($matches[1])-intval($matches[3]);
  $diff2=intval($matches[4])-intval($matches[2]);
  if($diff1==0 || $diff2==0 || sign($diff1) == sign($diff2))
    print "1\n";
  else 
    print "0\n";
}


?>
