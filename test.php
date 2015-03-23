<html>
<head>
<title>
</title>
<?php
 
?>

</head>
<body>

<?php
 
//function PHPfunction()
{ 
 
// 設定連線 URL
$url = 'http://127.0.0.1:8088/xxx.php';
preg_match('/^(.+:\/\/)([^:\/]+):?(\d*)(\/.+)/', $url, $matches);
$protocol = $matches[1];
$host = $matches[2];
$port = $matches[3];
$uri = $matches[4];
 
// 設定等等要傳送的資料 
$cmd = 'ConnectCam';
//$xml = 'CamIP=10.62.8.179/h264';
$xml = 'CamIP=10.62.8.103/ch4';
$xml .= '&CamUsr=Admin';
$xml .= '&CamPwd=1234';

// 開啟一個 TCP/IP Socket
$fp = fsockopen($host, $port, $errno, $errstr, 5);
if ($fp) {
    // 設定 header 與 body
    $httpHeadStr  = "POST /{$cmd} HTTP/1.1\r\n";
    $httpHeadStr .= "Content-type: text/html\r\n";
    $httpHeadStr .= "Host: {$host}:{$port}\r\n";
    $httpHeadStr .= "Content-Length: ".strlen($xml)."\r\n";
    $httpHeadStr .= "Connection: close\r\n";
    $httpHeadStr .= "\r\n";
    $httpBody = $xml."\r\n";
 
    // 呼叫 WebService
    fputs($fp, $httpHeadStr.$httpBody);
    $response = '';
    while (!feof($fp)) {
        $response .= fgets($fp, 2048);
    }
    fclose($fp);
 
    // 顯示回傳資料
    //echo $response;
	

	// split the result header from the content
    $result = explode("\r\n\r\n", $response, 2);
	$header = isset($result[0]) ? $result[0] : '';
    $content = isset($result[1]) ? $result[1] : '';
	echo $header."<br />";
	echo $content."<br />";

	$status = -1;
	$appName = 0;
	parse_str($content);
	echo $status."<br />";  // value
	echo $appName."<br />";  // value
	
	$appName = preg_replace('/\s+/', '', $appName);
//------------------------------	
	if($status==1)
	{
	
		$rmtpstr = '&src=rtmp://localhost/'.$appName.'/livestream&autoHideControlBar=true&streamType=live&autoPlay=true&verbose=true';
 
		echo "<object width='640' height='377' id='SampleMediaPlayback' name='SampleMediaPlayback' type='application/x-shockwave-flash' classid='clsid:d27cdb6e-ae6d-11cf-96b8-444553540000' >";
		echo "<param name='movie' value='SampleMediaPlayback.swf' />";
		echo "<param name='quality' value='high' />";
		echo "<param name='bgcolor' value='#000000' />" ;
		echo "<param name='allowfullscreen' value='true' />";

		echo "<param name='flashvars' value=".$rmtpstr."/>";
 
		echo "<embed src='SampleMediaPlayback.swf' width='640' height='377' 
		id='SampleMediaPlayback' quality='high' bgcolor='#000000' 
		name='SampleMediaPlayback' allowfullscreen='true' 
		pluginspage='http://www.adobe.com/go/getflashplayer' 
		flashvars=".$rmtpstr." type='application/x-shockwave-flash'> 
		</embed></object>";
 
 

		echo $rmtpstr."<br />";  // value
	}
//------------------------------
	
} else {
 
    die('Error:'.$errno.$errstr);
}

}
 
?>

 

</body>
</html>