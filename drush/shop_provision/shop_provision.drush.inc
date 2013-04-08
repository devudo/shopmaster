<?php
/**
 * @file shop_provision.drush.inc
 */

/**
 *
 */
function shop_provision_drush_commands(){
  $items['create-server'] = array(
    'description' => 'Fires up a new server with knife, using the specified chef role.', 
    'arguments' => array(
      'hostname' => 'The hostname to use for the server.',
    ),
    'options' => array(
      'runlist' => 'A chef runlist or role.'
    ),
  );
  return $items;
}
/*********
 * TASKS *
 ********/

/**
 * Implements drush_HOOK_pre_COMMAND()
 *   for provision-verify
 *
 *   $task NOT available
 *   d() is server, does NOT get data from pre_hosting_task()
 *
 *   @NOTE: We cannot access $task->ref during the provision-verify command.
 *   We cannot fire real errors in the hosting-task command.
 *   We are in a serious catch 22 here.
 *
 *   The provision-verify is in a totally separate process, so we cannot
 *   track any data from hosting-task and hosting-task hooks can't get any
 *   data from here.
 *
 *   So, for now, Tasks will always appear to complete "successfully", until
 *   we can figure out how to save data to the drush alias and then use
 *   this hook to access it and save it back to hostmaster.
 */
//function drush_shop_provision_pre_provision_verify() {
//  
//  drush_log('drush_shop_provision_pre_provision_verify', 'warning');
//  drush_log('d() = ' . print_r(d(), 1), 'warning');
//
//  $context = drush_get_context('alias');
//  drush_log('$context = '. print_r($context, 1), 'warning');
//
//  // On server verify: we will create or update our server
//  if (d()->context_type == 'server' && d()->name != '@server_localhost' && d()->name != '@server_master') {
//    drush_log('[DEVUDO] Verifying devudo server...', 'notice');
//    return shop_provision_verify_server();
//  }
//}

/**
 * Implements drush_HOOK_pre_COMMAND()
 *   for provision-delete
 */
//function drush_shop_provision_pre_provision_delete() {
//  drush_log('drush_shop_provision_pre_provision_verify', 'notice');
//  drush_log('d() = ' . print_r(d(), 1), 'notice');
//  
//  // On server delete, run the commands to actually destroy the server.
//  if (d()->context_type == 'server') {
//    drush_log('[DEVUDO] deleting devudo server...', 'notice');
//    return shop_provision_delete_server();
//  }
//}

/**
 * Verify Server: If no chef node by server name found, $ knife rackspace create
 */
function shop_provision_verify_server() {
  $task = &drush_get_context('HOSTING_TASK');

  // Get some vars and attributes
  $server_fqdn = $task->ref->title;
  $attributes = $task->ref->attributes;
  $json_path = "/tmp/$server_fqdn.json";
  
  // Look for this chef node on Chef Server
  drush_log("[DEVUDO] Looking for chef node $server_fqdn on chef server", 'warning');
  $chef_node = shop_get_server($server_fqdn);
  
  // CREATE: If no server found, Create & prepare a new server
  if (empty($chef_node)){
    
    // @TODO: Allow flavor to be chosen
    drush_log("[DEVUDO] Chef node not found with name:$server_fqdn. Creating server...", 'warning');
    
    // CREATE THE SERVER!
    $rackspace_id = shop_create_server($server_fqdn, $attributes);
    if ($rackspace_id){
      drush_log("[DEVUDO] Rackspace ID Detected: $rackspace_id", 'ok');
      
      // Save the Rackspace ID
      $task->ref->rackspace_id = $rackspace_id;  
    } else {
      
      // No rackspace ID? something went wrong.
      // @TODO!  We cannot set error here because this function is
      // being called during hosting-task, not provision-verify!
      // If we set error now, the TASK runs FOREVER!  WTF WTF WTF
      // return drush_set_error('DEVUDO_SERVER_CREATE', dt('Something went wrong trying to create a server on Rackspace.'));
      return drush_log(dt('[DEVUDO] Something went wrong trying to create a server on Rackspace.'), 'error');
    }
    
  }
  // UPDATE: If a server was found, we will update it's attributes
  else {
    
    drush_log("[DEVUDO] Chef node found with name:$server_fqdn", 'ok');
    
    // Save new json data to file
    $attributes_json = json_encode($attributes);
    file_put_contents($json_path, $attributes_json);
    
    // Sync file to server
    // Use IP in case something is wrong with DNS
    if (!empty($task->ref->ip_addresses)){
      $host = array_pop($task->ref->ip_addresses);
    } else {
      $host = $server_fqdn;
    }
    shop_exec("scp $json_path devudo@$host:~/attributes.json");

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
  //if (!empty($chef_node->automatic->rackspace)){
  //  drush_log('[DEVUDO] Rackspace Info found', 'ok');
  //  $task->ref->rackspace = $chef_node->automatic->rackspace;
  //} else {
  //  
  //}
  
  // @TODO: If this was a livemaster server with a devmaster parent, grab the
  // livemaster mysql root password and devmaster_parent_ip, and save it to the
  // server context! (drush alias)
  // Then, sync the server alias to the devmaster_parent server, then run
  // @hostmater hosting-import "@server_NAME"!  Then, if all goes well,
  // Devmaster will verify the server, be able to connect and create DBs, and
  // restart NGINX!
}


/**
 * Delete Server: $ knife rackspace server delete
 */
function shop_provision_delete_server(){
  
  // Get some vars and attributes
  $server_fqdn = d()->remote_host;
  $attributes = d()->attributes;
  $server_id = $attributes->devudo->rackspace_id;
  
  $cmd = "knife rackspace server delete $server_id -P -y";
  drush_log("[DEVUDO] Attempting to delete server $server_fqdn, Rackspace ID: $server_id" );
  
  if (!$server_id) {
    //return drush_set_error('DEVUDO_RACKSPACE_ERROR', dt('No server ID!  Without that, I cannot delete the server.'));
    return drush_log(dt('No server ID!  Without that, I cannot delete the server.'), 'error');
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
    //return drush_set_error('DRUSH_FRAMEWORK_ERROR', dt('Something went wrong when trying to tell rackspace to delete the server! Please review the logs above.'));
    drush_log(dt('Something went wrong when trying to tell rackspace to delete the server! Please review the logs above.'), 'error');
    return FALSE;
  }
  // Tell drush_shop_hosting_pre_hosting_task() that we successfully deleted!
  return TRUE;
}

/*********
 * TOOLS *
 ********/

/**
 * Helpful Exec
 */
function shop_exec($command, $label = "EXEC", $command_label = "KNIFE", $output = 'string'){
  drush_log("[$label] Running: $command");
  _drush_shell_exec(array($command), TRUE);
  $lines = drush_shell_exec_output();
  foreach ($lines as $line){
    //drush_log("[$command_label] $line");
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
function shop_create_server($server_fqdn, $attributes = array()){
  $role = $attributes->devudo['chef_role'];
  $rackspace_flavor = $attributes->devudo['rackspace_flavor'];

  // If no rackspace flavor it is doomed
  if (!$rackspace_flavor) {
    //return drush_set_error('DEVUDO_RACKSPACE_CREATE', dt('No rackspace_flavor found!'));
    return drush_log(dt('No rackspace_flavor found!'), 'error');
  }
  
  $attributes_json = escapeshellarg(json_encode($attributes));
  
  // @TODO: A nice array of options would make sense here.
  $run_list = ($role)? "role[$role]": "";
  $options = ($run_list)? "-r $run_list": "";
  $options .= " --flavor $rackspace_flavor ";
  $lines = shop_exec("knife rackspace server create $options --server-name $server_fqdn --image 5cebb13a-f783-4f8c-8058-c4182c724ccd -Fjson -j $attributes_json", "KNIFE rackspace", "CHEF", 'array');
  
  // Detect an error:
  if (strpos($lines[0], 'ERROR') === 0 || strpos($lines[0], 'FATAL') === 0){
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
