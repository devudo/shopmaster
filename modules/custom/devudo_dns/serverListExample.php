<?php

/**
 * Test run like so:
 *  $  php serverListExample.php
 */

  $_ENV['OS_USERNAME'] = 'careernerd';
  $_ENV['OS_TENANT_NAME'] = 'devudo';
  $_ENV['NOVA_API_KEY'] = 'e93596076f1a3bd404d6a8b790b8a96b';

require_once "lib/php-opencloud.php";

define('AUTHURL', RACKSPACE_US);
define('USERNAME', $_ENV['OS_USERNAME']);
define('TENANT', $_ENV['OS_TENANT_NAME']);
define('APIKEY', $_ENV['NOVA_API_KEY']);

// establish our credentials
$connection = new \OpenCloud\Rackspace(AUTHURL,
	array( 'username' => USERNAME,
		   'apiKey' => APIKEY ));

// now, connect to the compute service
$compute = $connection->Compute('cloudServersOpenStack', 'DFW');

// list all servers
print("ALL SERVERS:\n");
$slist = $compute->ServerList();
while($server = $slist->Next()){ 
    printf("* %-20s %-10s (%s)\n", 
		$server->Name(), $server->status, $server->ip());
}