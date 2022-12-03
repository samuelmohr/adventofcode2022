<?php

$scores=array();
for($i=1;$i<=26;$i++){
  $scores[sprintf("%c",$i+96)]=$i;
  $scores[sprintf("%c",$i+64)]=$i+26;
}

$stdin = fopen('php://stdin','r');
while( $line = fgets( $stdin ) ) {
  $a1=str_split($line);
  sort($a1);
  $line = fgets( $stdin );
  $a2=str_split($line);
  sort($a2);
  $line = fgets( $stdin );
  $a3=str_split($line);
  sort($a3);  
  $all=array_merge($a1,$a2,$a3);
  sort($all);
  $score=0;
  while(count($a1)+count($a2)+count($a3)>0){
    $char=array_pop($all);
    if($a1[count($a1)-1]===$char) {
      array_pop($a1);
      if($a2[count($a2)-1]===$char && $a3[count($a3)-1]===$char ){
        $score=$scores[$char];
        break;
      }
    } elseif($a2[count($a2)-1]===$char) {
      array_pop($a2);
    } else {
      array_pop($a3);
    }
  }
  print $score;
  print "\n";
}


?>
