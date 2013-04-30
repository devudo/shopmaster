<?php
/**
 * @file devudo.drush.inc
 *
 */

/**
 * Implements hook_provision_services()
 * 
 *  Declares a new service type and a basic implementation of it.
 *  It matches the same service definition in the hosting front end.
 */
function devudo_provision_services() {
  require_once('devudo.service.inc');
  return array('devudo' => 'servers');
}


/*********
 * TOOLS *
 ********/

/**
 * Helpful Exec
 */
function shop_exec($command){
  $lines = array();
  drush_log("[DEVUDO] Running: $command", "ok");
  exec($command, $lines);
  foreach ($lines as $line){
    drush_log("[EXEC] $line", 'ok');
  }
}

/**
 * Helpful Exec
 */
function shop_exec_return($command, $output = 'array'){
  $lines = array();
  drush_log("[DEVUDO] Running: $command", "ok");
  exec($command, $lines);
  foreach ($lines as $line){
    //drush_log("[EXEC] $line", 'ok');
  }
  return $output == 'string'? implode("\n", $lines): $lines;
}
/**
 * Helpful Exec to JSon
 */
function shop_exec_json($command, $return_string = FALSE, $label = "EXEC"){
  drush_log("[$label] Running: $command");
  drush_shell_exec($command);
  $lines = drush_shell_exec_output();
  $data_string = implode($lines);
  $data_json = json_decode($data_string);
  
  // Force return string if json did not decode
  if (!is_array($data_json) && !is_object($data_json)){
    return $data_string;
  } else {
    // If asked to return a string, return a string.
    return $return_string? $data_string: $data_json;
  }
  
  
}

/**
 * Helper to load a server node from chef server by name
 */
function shop_get_server($node_name, $options = ' -l'){
  return shop_exec_json("knife node show $node_name -Fj $options", FALSE, 'KNIFE');
}
//
///**
// * Helper to create a server from scratch.  Runs knife rackspace server create 
// */
//function shop_create_server($server_fqdn, $attributes = array()){
//  $role = $attributes->devudo['chef_role'];
//  $rackspace_flavor = $attributes->devudo['rackspace_flavor'];
//
//  // If no rackspace flavor it is doomed
//  if (!$rackspace_flavor) {
//    return drush_log(dt('No rackspace_flavor found!'), 'error');
//  }
//  
//  $attributes_json = json_encode($attributes);
//
//  drush_log("[DEVUDO] Running: drush server-create $server_fqdn $role --rackspace_flavor $rackspace_flavor --attributes=$attributes_json");
//  
//  $args[] = $server_fqdn;
//  $args[] = $role;
//  $options['rackspace_flavor'] = $rackspace_flavor;
//  $options['attributes'] = $attributes_json;
//  
//  // Invoking new process.
//  
//  //drush_invoke_process('@none', 'server-create', array($server_fqdn, $role),  array('rackspace_flavor' => $rackspace_flavor, 'attributes'=> $attributes_json));
//                                           
//  drush_set_option('rackspace_flavor', $rackspace_flavor);
//  drush_set_option('attributes', $attributes_json);
//  $data = drush_shop_provision_server_create($server_fqdn, $role);
//  return $data;
//}

/**
 * Helper to log an object
 */
function shop_log_object($object, $name = '', $label = 'DEVUDO'){
  $object = print_r($object, 1);
  drush_log("[$label] $name => $object");
}
