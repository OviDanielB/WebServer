<?php
/**
 * Process PHP:
 * 1) opening FIFO
 * 2) waiting for something to read
 * 3) reading message (user-agent) and use it to query wurfl.xml
 * 4) elaborating query results and write back the response to other process
 */

define('FIFO_FILE', '/tmp/');
include_once 'wurfl-php-1.7.1.1/examples/demo/inc/wurfl_config_standard.php';

// contains all fifo file streams for reading (the other process is writing)
$original_r = array();
// contains all fifo file streams for writing (the other process is reading)
global $original_w ;
$original_w = array();

if($argc == 1){
    echo "Usage: fifo.php <pids>...\n";
    exit(0);
}

for($i = 0; $i < $argc - 1; $i++) {
    $original_r[$i] = fopen(FIFO_FILE . $argv[$i + 1] . "w", 'r+');

    $original_w[$i] = fopen(FIFO_FILE . $argv[$i + 1] . "r", 'w+');

    if( ! ($original_r[$i] || $original_w[$i]) ) {
        die('Error: Could not open the named pipe: '. posix_strerror(posix_errno()) . "\n");
    }
}

$wurlf = $wurflManager;

echo "Pipe(s) successfuly opened for reading!\n";

// Original array of sockets
$r = $original_r;
$w = null;
$e = null;
$s = null;

while (stream_select($r, $w , $e , $s ) !== 0)
{
    
        foreach ($r as $item) {

            $output = fgets($item);

            if($output != '') {
                /*
                echo 'Received from the pipe ' . count($r) . ' : ' . $output . "\n";
                echo 'Device ID : ' . $device->getCapability('brand_name') . " " . $device->getCapability("model_name")  . "\n";
                echo 'Device Height : ' . $device->getCapability('resolution_height') . "\n";
                echo 'Device Width : ' . $device->getCapability('resolution_width') . "\n";
                echo 'Device Colors : ' . $device->getCapability('colors') . "\n";
                echo 'Format support PNG : ' . $device->getCapability('png') . "\n";
                echo 'Format support JPEG : ' . $device->getCapability('jpg') . "\n";
                echo 'Format support GIF : ' . $device->getCapability('gif') . "\n";
                */

                $device = $wurlf->getDeviceForUserAgent($output);

                $string = compose_response($device);
                echo($string);
                $w_fifo =  getWriteFifo($item,$original_w);

                fwrite($w_fifo,$string);
            }
        }

        // stream_select modifies the contents of $r
        // in a loop we should replace it with the original
        $r = $original_r;
}

/* Find writing FIFO associated to process that interrupt the select   */
function getWriteFifo($read_fifo, $write_array){
    $meta = stream_get_meta_data($read_fifo);

    $wFifo_name = str_replace("w","r",$meta['uri']);

    foreach ($write_array as $value) {
        $meta_d = stream_get_meta_data($value);
        if($wFifo_name == $meta_d['uri']){
            return $value;
        }
    }
}

/** Composing response message to write on FIFO
 *
 * @param $device WURFL_CustomDevice got by wurfl query
 * @return $final string
 */
function compose_response($device ){
    $final = "";
    $final .=  "?!" . $device->getCapability('brand_name') . $device->getCapability('model_name') . "|";
    $final .= $device->getCapability('resolution_width') . "|" . $device->getCapability('resolution_height') . "|";
    $final .= $device->getCapability('colors') . "|";
    $final .= $device->getCapability('jpg') . "|" . $device->getCapability('png') . "|" . $device->getCapability('gif') . "?!";

    return $final;
}
?>