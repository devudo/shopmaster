<?php

/**
 * Test run like so:
 *  $  php serverListExample.php
 */

  $_ENV['OS_USERNAME'] = 'careernerd';
  $_ENV['OS_TENANT_NAME'] = 'devudo';
  $_ENV['NOVA_API_KEY'] = 'e93596076f1a3bd404d6a8b790b8a96b';

require_once "lib/php-opencloud.php";
require_once "lib/Autoload.php";


define('AUTHURL', RACKSPACE_US);
define('USERNAME', $_ENV['OS_USERNAME']);
define('TENANT', $_ENV['OS_TENANT_NAME']);
define('APIKEY', $_ENV['NOVA_API_KEY']);

printf("Authenticating...\n");

// establish our credentials
$cloud = new \OpenCloud\Rackspace(AUTHURL,
	array( 'username' => USERNAME,
		   'apiKey' => APIKEY ));
//setDebug(TRUE);

printf("Connecting to DNS...\n");
$dns = $cloud->DNS();


// This gets a \OpenCloud\Collection, but I don't know how to use
// it to get the domain ID...
// but the ID for devudo.com is 3428941
// $list = $dns->DomainList(array('name' => 'devudo.com'));

// Gets a Domain object
//$domain = $dns->Domain(3428941);
$domain = $dns->Domain(3428941);

// Create main domain record
$record = $domain->Record();
$resp = $record->Create(array(
	'type' => 'A',
	'name' => 'mynewsite.devudo.com',
	'ttl' => 600,
	'data' => '10.10.10.10'
));
$resp->WaitFor("COMPLETED", 300, 'showme', 1);


// Create wildcard domain record
$resp = $record->Create(array(
	'type' => 'A',
	'name' => '*.mynewsite.devudo.com',
	'ttl' => 600,
	'data' => '10.10.10.10'
));

$resp->WaitFor("COMPLETED", 300, 'showme', 1);
exit;

// callback for WaitFor method
function showme($obj) {
	printf("%s %s %s\n", date('H:i:s'), $obj->Status(), $obj->Name());
	if ($obj->Status() == 'ERROR') {
		printf("\tError code [%d] message [%s]\n\tDetails: %s\n",
			$obj->error->code, $obj->error->message, $obj->error->details);
	}
	else if ($obj->Status() == 'COMPLETED') {
		printf("Done\n");
	}
}