<?php
/**
 * @file shop_provision.drush.inc
 */

define('SHOP_PROVISION_ERROR', 1);
 
/**
 * Implements drush_HOOK_pre_COMMAND()
 *
 * This runs for each task during the command
 *    drush @hostmaster hosting-tasks
 *
 * NOTE: This ONLY runs when being called from a hostmaster task,
 * (ie. via the Hostmaster Drupal Front-end)
 *
 * The server node is available at $task->ref.
 * This node is saved at the end!
 */
function drush_shop_provision_pre_hosting_task() {
  // Get Hostmaster Task object
  $task =& drush_get_context('HOSTING_TASK');
  // On server delete, run the commands to actually destroy the server.
  if ($task->ref->type == 'server' && $task->task_type == 'delete') {
    drush_shop_provision_pre_hosting_task_server_delete();
  }
  
  // On server verify: we will create or update our server
  if ($task->ref->type == 'server' && $task->task_type == 'verify') {
    drush_shop_provision_pre_hosting_task_server_verify();
  }
}

/**
 * Delete Server: $ knife rackspace server delete
 */
function drush_shop_provision_pre_hosting_task_server_delete(){
  $task =& drush_get_context('HOSTING_TASK');
  $server_fqdn = $task->ref->title;
  $server_id = $task->ref->devudo_server_info->rackspace_id;
  
  $cmd = "knife rackspace server delete $server_id -P -y";
  drush_log("[DEVUDO] Attempting to deleting server $server_fqdn, Rackspace ID: $server_id" );
  
  if (!$server_id) {
    drush_log('No server ID!  Without that, I cannot delete the server.', 'error');
    return;
  }
  $lines = shop_exec($cmd, 'RUNNING', 'KNIFE', 'lines');
  // When successful, the last three lines look like this:
  // [KNIFE] WARNING: Deleted server bb23f28a-c1fc-4e49-9c15-48b1807dc08c
  // [KNIFE] WARNING: Deleted node ben.devudo.com
  // [KNIFE] WARNING: Deleted client ben.devudo.com
  $last_lines = array();
  $last_line[] = array_pop($lines);
  $last_line[] = array_pop($lines);
  $last_line[] = array_pop($lines);
  if ($last_line[2] == "[KNIFE] WARNING: Deleted server $server_id") {
    drush_log("[DEVUDO] Server Deletion successful!", 'ok');
  } else {
    drush_log("last line == ? " . $last_line[2], 'notice');
    drush_log('Something went wrong when trying to tell rackspace to delete the server! Please review the logs above.', 'error');
  }
  
}

/**
 * Verify Server: If no chef node by server name found, $ knife rackspace create
 */
function drush_shop_provision_pre_hosting_task_server_verify() {
  $task =& drush_get_context('HOSTING_TASK');

  // Get some vars and attributes
  $server_fqdn = $task->ref->title;
  $attributes = $task->ref->attributes;
  $json_path = "/tmp/$server_fqdn.json";
  
  // Look for this chef node on Chef Server
  drush_log("[DEVUDO] Looking for chef node $server_fqdn on chef server", 'ok');
  $chef_node = shop_get_server($server_fqdn);
  
  // @TODO: THIS (the node_load) DOES NOT ALWAYS WORK! field_chef_role not avail.
  // Happened on first verify of a new devmaster server.
  // BUT!! When it didn't work, the TASK COMPLETED SUCCESSFULLY.  If this DOES
  // load the TASK NEVER FINISHES.... could be a clue.
  $server_node = node_load($task->ref->nid);
  $chef_role = $server_node->field_chef_role[0]['value'];
  $rackspace_flavor = $server_node->field_rackspace_flavor[0]['value'];
  
  // CREATE: If no server found, Create & prepare a new server
  if (empty($chef_node)){
    
    // @TODO: Allow flavor to be chosen
    drush_log("[DEVUDO] Chef node not found with name:$server_fqdn. Creating server...", 'warning');
    $rackspace_id = shop_create_server($server_fqdn, $chef_role, $rackspace_flavor, $attributes);
    drush_log("[DEVUDO] Rackspace ID Detected: $rackspace_id", 'ok');
    $task->ref->rackspace_id = $rackspace_id;
  }
  // UPDATE: If a server was found, we will update it's attributes
  else {
    
    // @TODO: If we allow changing of role we would do it here...
    
    drush_log("[DEVUDO] Chef node found with name:$server_fqdn", 'ok');
    
    // Save new json data to file
    $attributes_json = json_encode($attributes);
    file_put_contents($json_path, $attributes_json);
    
    // Sync file to server
    shop_exec("scp $json_path devudo@$server_fqdn:~/attributes.json");

    // Run chef-client to update the server itself.
    $chef_client_cmd = "sudo /usr/bin/chef-client -j attributes.json";
    $chef_client_cmd_exec = escapeshellarg($chef_client_cmd);
    drush_log("[DEVUDO] Running chef-client on $server_fqdn:", 'notice');
    shop_exec("knife ssh name:$server_fqdn -x devudo $chef_client_cmd_exec -a ipaddress");
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
 * Helper to load a server node from chef server by name
 */
function shop_get_server($node_name, $options = ' -l'){
  return shop_exec_json("knife node show $node_name -Fj $options", FALSE, 'KNIFE');
}

/**
 * Helper to create a server from scratch.  Runs knife rackspace server create 
 */
function shop_create_server($server_fqdn, $role = NULL, $rackspace_flavor = 2, $attributes = array()){
  // If no rackspace flavor it is doomed
  if (!$rackspace_flavor) {
    drush_log('No rackspace_flavor found!', 'error');
    return;
  }
  
  $attributes_json = escapeshellarg(json_encode($attributes));
  
  // @TODO: A nice array of options would make sense here.
  $run_list = ($role)? "role[$role]": "";
  $options = ($run_list)? "-r $run_list": "";
  $options .= " --flavor $rackspace_flavor ";
  $lines = shop_exec("knife rackspace server create $options --server-name $server_fqdn --image 5cebb13a-f783-4f8c-8058-c4182c724ccd -Fjson -j $attributes_json", "KNIFE", "CHEF", 'array');
  
  // Detect an error:
  if (strpos($lines[0], 'ERROR') === 0){
    drush_log('Error from $ knife rackspace server create: '. $lines[0], 'error');
    return NULL;
  }
  
  // We are grabbing this here because, surprisingly, chef doesn't store the rackspace ID
  $rackspace_instance_id = str_replace('Instance ID: ', '', $lines[0]);
  
  // Make sure run list is applied.
  shop_exec("knife node run-list add $server_fqdn $run_list");
 
  // @TODO confirm the server exists with a knife node show.
  shop_exec("knife node show $server_fqdn");
  
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
