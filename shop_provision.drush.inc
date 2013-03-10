<?php

//provision-save


/**
 * Implements drush_HOOK_pre_COMMAND()
 *
 * This runs for each tasks during the command
 *    drush @hostmaster hosting-tasks
 *
 * NOTE: This ONLY runs when being called from a hostmaster task.
 *
 * The server node is available at $task->ref.
 * This node is saved at the end!
 */
function drush_shop_provision_pre_hosting_task() {
  // Get Hostmaster Task object
  $task =& drush_get_context('HOSTING_TASK');
  // NOT READY YET
  //// On server delete, run the commands 
  if ($task->ref->type == 'server' && $task->task_type == 'delete') {
    drush_shop_provision_pre_hosting_task_server_delete();
  }
  
  // On server verify: we will create or update our server
  if ($task->ref->type == 'server' && $task->task_type == 'verify') {
    drush_shop_provision_pre_hosting_task_server_verify();
  }
}

/**
 * Pre Server Delete
 */
function drush_shop_provision_pre_hosting_task_server_delete(){
  $task =& drush_get_context('HOSTING_TASK');
  $server_fqdn = $task->ref->title;
  $server_id = $task->ref->devudo_server_info->rackspace_id;
  
  $cmd = "knife rackspace server delete $server_id -P -y";
  drush_log("[DEVUDO] Deleting server $server_fqdn");
  shop_exec($cmd);
}

/**
 * Pre Server Verify
 */
function drush_shop_provision_pre_hosting_task_server_verify() {
  $task =& drush_get_context('HOSTING_TASK');

  $attributes = $task->ref->attributes;
  $json_path = "/tmp/$server_fqdn.json";
  
  // Look for this chef node on Chef Server
  $server_fqdn = $task->ref->title;
  drush_log("[DEVUDO] Looking for chef node $server_fqdn on chef server", 'ok');
  $chef_node = shop_get_server($server_fqdn);
  
  // CREATE: If no server found, Create & prepare a new server
  if (empty($chef_node)){
    
    // @TODO: Allow flavor to be chosen
    drush_log("[DEVUDO] Chef node not found with name:$server_fqdn. Creating server...", 'warning');
    $rackspace_id = shop_create_server($server_fqdn, $attributes);
    drush_log("[DEVUDO] Rackspace ID Detected: $rackspace_id", 'ok');
    $task->ref->rackspace_id = $rackspace_id;
  }
  // UPDATE: If a server was found, we will update it's attributes
  else {
    drush_log("[DEVUDO] Chef node found with name:$server_fqdn, preparing to update attributes...", 'ok');
    
    // Save chef normal attributes from shop.devudo.com server node
    // Lets do both here!  better safe then sorry?
    // $chef_node->override = $attributes;
    $chef_node->normal = $attributes;

    // Save new json data to file
    $chef_node_string = json_encode($chef_node);
    file_put_contents($json_path, $chef_node_string);
    //drush_register_file_for_deletion($json_path);
    
    // Save chef node (on chef server) from file.
    // @TODO: I think this is causing the loss of system attributes 
    drush_log('[DEVUDO] Saving chef node from file...', 'ok');
    shop_exec("knife node from file $json_path");
    
    // Run chef-client to update the server itself.
    // NOTE: We don't have to do this on create, I THINK
    drush_log('[DEVUDO] Running chef-client to update the chef node.', 'ok');
    shop_exec("knife ssh name:$server_fqdn -x devudo 'sudo /usr/bin/chef-client' -a ipaddress");
  }
  
  // Get the newly created or updated chef node from chef server
  $chef_node = shop_get_server($server_fqdn);
  
  // If we have the IP, save it here
  // @TODO: Should we grab both public and private IPs?
  if (!empty($chef_node->automatic->ipaddress)){
    drush_log('[DEVUDO] ipaddress found in chef node attributes!', 'ok');
    $task->ref->ip_addresses = array($chef_node->automatic->ipaddress);
    
    // @TODO: SAVE DNS!!! This will be much slower here...
    // We need to add the DNS record the moment that rackspace
    // grants an IP address
    
  } else {
    drush_log('[DEVUDO] No ipaddress found in chef node attributes!', 'warning');
  }
  
  // @TODO: Save other important attributes now.
  // NOTE: The actual rackspace server ID does NOT seem to be here!
  //   Lets research the right way to get it or just hack our way to
  //   it by using knife rackspace list.
  if (!empty($chef_node->automatic->rackspace)){
    drush_log('[DEVUDO] Rackspace Info found', 'ok');
    $task->ref->rackspace = $chef_node->automatic->rackspace;
  } else {
    
  }
}

/**
 * Helpful Exec
 */
function shop_exec($command, $label = "EXEC", $command_label = "KNIFE", $output = 'string'){
  drush_log("[$label] Running: $command");
  drush_shell_exec($command);
  $lines = drush_shell_exec_output();
  foreach ($lines as $line){
    drush_log("[$command_label] $line");
  }
  return $output == 'string'? implode("\n", $lines): $lines;
}

/**
 * Helpful Exec to JSon
 */
function shop_exec_json($command, $string = FALSE, $label = "EXEC"){
  drush_log("[$label] Running: $command");
  drush_shell_exec($command);
  $lines = drush_shell_exec_output();
  $json = implode($lines);
  drush_log("[$label] Returned: $json");
  return $string? $json: json_decode($json);
}

/**
 * Helper to load a server node from chef server
 */
function shop_get_server($node_name, $options = ' -l'){
  return shop_exec_json("knife node show $node_name -Fj $options", FALSE, 'KNIFE');
}

/**
 * Helper to create a server from scratch
 */
function shop_create_server($server_fqdn, $attributes = array()){
  $attributes_json = escapeshellarg(json_encode($attributes));
  $lines = shop_exec("knife rackspace server create -r 'role[devmaster]' --server-name $server_fqdn --image 5cebb13a-f783-4f8c-8058-c4182c724ccd --flavor 2 -Fjson -j $attributes_json", "KNIFE", "CHEF", 'array');
  
  // @TODO: Will the first line always be our instance id?
  $rackspace_instance_id = str_replace('Instance ID: ', '', $lines[0]);
  return $rackspace_instance_id;
}

/**
 * Helper to log an object
 */
function shop_log_object($object, $name = '', $label = 'DEVUDO'){
  $object = print_r($object, 1);
  drush_log("[$label] $name => $object");
}
/**
 * Saving  to $task->context_options['ip_addresses'] works!
 */
function shop_provision_hosting_server_context_options(&$task) {
  
  drush_log("[DEVUDO] Shopmaster context options: " . $task->devudo_test, 'ok');
  
  $task->context_options['remote_host'] = $task->ref->title;
  $ip_list = _hosting_ip_list($task->ref->ip_addresses);
  $task->context_options['ip_addresses'] = sizeof($ip_list) ? implode(',', $ip_list) : 'null';

}