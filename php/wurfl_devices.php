<?php
/**
 * Created by PhpStorm.
 * User: ovidiudanielbarba
 * Date: 11/05/16
 * Time: 14:47
 */

/*
 * CAPABILITIES:
 * ID,COLORS;WIDTH;HEIGHT;
 * ARRAY CHAR * (PNG, JPEG , GIF support)
 *
 * */

include_once 'wurfl-php-1.7.1.1/examples/demo/inc/wurfl_config_standard.php';



$wurflInfo = $wurflManager->getWURFLInfo();

$wurflXMLDeviceIterator = new WURFL_Xml_DeviceIterator('wurfl.xml', array());
$wurflXMLDeviceIterator->rewind();

$userAgent = "Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:42.0) Gecko/20100101 Firefox/42.0";
$requestingDevice = $wurflManager->getDeviceForUserAgent($userAgent);
echo "Width :" . $requestingDevice->getCapability("resolution_width");

/*

for($i = 0; $i< 100; $i++){
    $wurflXMLDeviceIterator->readNextElement();
    $currentDevice = $wurflXMLDeviceIterator->currentElement;

    echo "Name : " . $currentDevice->getCapability("brand_name") . " " . $currentDevice->getCapability("model_name") .  "\n";
    echo "Height : " . $currentDevice->getCapability("resolution_height"). "\n";
    echo "Width :" . $currentDevice->getCapability("resolution_width") . "\n";
    echo "OS : " . $currentDevice->getCapability("device_os") . "\n\n";
}

*/
?>