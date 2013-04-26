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

// Get a list of domains named "devudo.com" (there will only be one.)
$dlist = $dns->DomainList(array('name' => 'devudo.com'));
while($domain = $dlist->Next()) {
	printf("\n%s [%s] %s\n",
		$domain->Name(), $domain->emailAddress, $domain->id);
  // Grab the domain ID
  if ($domain->Name() == 'devudo.com'){
    $domain_id =   $domain->id;
  }
}

// Gets a Domain object
//$domain = $dns->Domain(3428941);
$domain = $dns->Domain($domain_id);

// Create main domain record
$record = $domain->Record();
$resp = $record->Create(array(
	'type' => 'A',
	'name' => 'testdomains.devudo.com',
	'ttl' => 600,
	'data' => '10.10.10.10'
));
$resp->WaitFor("COMPLETED", 300, 'showme', 1);


// Create wildcard domain record
$resp = $record->Create(array(
	'type' => 'A',
	'name' => '*.testdomains.devudo.com',
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