<?php
$phone = "13810358035";
$cat_major = "1";
$style = "0";
$domain = "bj.ganji.com";
$url = "http://10.126.107.2:20201/img_url?phone=" . $phone . "&cat_major=" . $cat_major . "&domain=" . $domain . "&style=" . $style
;
echo $url;


$img_url = file_get_contents($url);
echo "<html><a href=" . $img_url . ">haha</a></html>"
#$img_bin = file_get_contents($img_url);
#header("Content-type: image/png");
#print_r($img_bin);

?>
