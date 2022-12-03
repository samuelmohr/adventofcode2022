<?php

$scores=array();
for($i=1;$i<=26;$i++){
  $scores[sprintf("%c",$i+96)]=$i;
  $scores[sprintf("%c",$i+64)]=$i+26;
}

$stdin = fopen('php://stdin','r');
while( $line = fgets( $stdin ) ) {
  $s1=substr($line,0,strlen($line)/2);
  $s2=substr($line,strlen($line)/2);
  $a1=str_split($s1);
  sort($a1);
  $a2=str_split($s2);
  sort($a2);
  $all=str_split($line);
  sort($all);
  $score=0;
  while(count($a1)+count($a2)>0){
    $char=array_pop($all);
    if($a1[count($a1)-1]===$char) {
      array_pop($a1);
      if($a2[count($a2)-1]===$char){
        $score=$scores[$char];
        break;
      }
    } else {
      array_pop($a2);
    }
  }
  print $score;
  print "\n";
}


?>
