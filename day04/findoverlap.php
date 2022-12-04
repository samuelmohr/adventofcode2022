<?php

function sign($n) {
    return ($n > 0) - ($n < 0);
}

$stdin = fopen('php://stdin','r');
while( $line = fgets( $stdin ) ) {
  if(!preg_match('/^(\d+)-(\d+),(\d+)-(\d+)$/', $line, $matches))
    continue;
  $diff1=intval($matches[1])-intval($matches[4]);
  $diff2=intval($matches[3])-intval($matches[2]);
  if(sign($diff1*$diff2)>=0)
    print "1\n";
  else 
    print "0\n";
}


?>
