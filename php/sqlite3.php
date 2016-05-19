<?php

    

	class MyDB extends SQLite3{

	function __construct(){
	$this->open('test.db');
	}
}

   

	$db = new MyDB();

	if(!$db){
	echo $db->lastErrorMsg();
	} else {
	echo "DB opened\n";
	}

	$sql = <<<EOF
      CREATE TABLE DEVICES
      (ID TEXT PRIMARY KEY     NOT NULL,
      UserAgent           TEXT    NOT NULL,
      Width            INT     NOT NULL,
      Height        INT NOT NULL );
EOF;

    $insert = <<<EOF
    INSERT INTO DEVICES(0, "Hello", 100,200);
EOF;

   $ret = $db->exec($sql);
   if(!$ret){
      echo $db->lastErrorMsg();
   } else {
      echo "Table created successfully\n";
   }
   $db->close();
?>


