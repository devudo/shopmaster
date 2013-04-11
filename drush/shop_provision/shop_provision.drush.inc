<?php
/**
 * @file shop_provision.drush.inc
 */

/**
 * Implements hook_drush_command.
 *
 * drush server create HOSTNAME ROLE --rackspace_flavor=2
 */
function shop_provision_drush_command(){
  $items['server-create'] = array(
    'description' => 'Fires up a new server with knife, using the specified chef role.', 
    'arguments' => array(
      'hostname' => 'The hostname to use for the server.',
      'role' => 'The chef role to use for this server.',
    ),
    'options' => array(
      'attributes' => 'A JSON string of attributes.',
      'rackspace_flavor' => 'The id of the Rackspace flavor you want to use. (2 through 8)',
      'rackspace_image' => 'The image of the Rackspace flavor you want to use.'
    ),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
  );
  return $items;
}

/**
 * Basic wrapper for knife rackspace server create.
 *
 * "knife rackspace server create $options --server-name $server_fqdn --image 5cebb13a-f783-4f8c-8058-c4182c724ccd -Fjson -j $attributes_json", "KNIFE CREATE", "EXEC", 'array');
 *
 * Returns the short data list that is found when chef is done.
 */
function drush_shop_provision_server_create($server_fqdn, $role = ""){
  drush_log("Hi! we're here to make a $role server called $server_fqdn!", 'ok');
  
  $rackspace_flavor = drush_get_option('rackspace_flavor', '2');
  $rackspace_image = drush_get_option('rackspace_image', '5cebb13a-f783-4f8c-8058-c4182c724ccd');
  
  $attributes_json = drush_get_option('attributes', '{}');
  $attributes_json = escapeshellarg($attributes_json);
  
  $run_list = ($role)? "role[$role]": "";
  $run_list = ($role)? "role[$role]": "";
  $options = ($run_list)? "-r $run_list": "";
  $options .= " --flavor $rackspace_flavor ";
  
  // Not using shpo_exec here to try and avoid these logging errors.
  $cmd = "knife rackspace server create $options --server-name $server_fqdn --image $rackspace_image -Fjson -j $attributes_json";
  
  drush_log("[EXEC] Running: $cmd", 'ok');
  drush_shell_exec($cmd);
  
  /**
   * SIMULATE
    drush_log("[EXEC] JUST KIDDING!  Moving on...", 'ok');
    $data['Instance ID'] =  'FAke33-eee3-47ee-b52d-RACk3RIDc24b';
    $data['Public IP Address'] =  "111.111.111.111";
    return $data;
   */
  
  // Extract some info.
  $lines = drush_shell_exec_output();
  
    //Instance ID: 051b356e-e6b3-47ee-b52d-a0cc03fdc24b
    //Host ID: e8df6854910f156a954ed00f55b2ededf5ea22529c84a4f7165a14cd
    //Name: devmaster.localhost
    //Flavor: 512MB Standard Instance
    //Image: Ubuntu 12.04 LTS (Precise Pangolin)
    //Public DNS Name: 166-78-61-124.static.cloud-ips.com
    //Public IP Address: 166.78.61.124
    //Private IP Address: 10.182.24.87
    //Password: BoBPErQFh2Lf
    //Environment: _default
    //Run List: role[devmaster]
  
  $data = array();
  $data_lines = array_slice($lines, -11);
  foreach ($data_lines as $line){
    drush_log($line, 'ok');
    list($key, $value) = explode(": ", $line);
    $data[$key] = $value;
  }
  
  // Make sure run list is applied.
  shop_exec("knife node run-list add $server_fqdn $run_list");
  return $data;
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
    $data = shop_create_server($server_fqdn, $attributes);
    $rackspace_id = $data['Instance ID'];
    $ipaddress =  $data['Public IP Address'];
    
    if (!empty($rackspace_id)){
      drush_log("[DEVUDO] Server Created! $ipaddress  $rackspace_id", 'ok');
      
      // Save the Rackspace ID
      $task->ref->rackspace_id = $rackspace_id;  
    } else {
      
      // No rackspace ID? something went wrong.
      // @TODO!  We cannot set error here because this function is
      // being called during hosting-task, not provision-verify!
      // If we set error now, the TASK runs FOREVER!  WTF WTF WTF
      // return drush_set_error('DEVUDO_SERVER_CREATE', dt('Something went wrong trying to create a server on Rackspace.'));
      return drush_log(dt('[DEVUDO] Rackspace ID not found!'), 'error');
    }
    
    // Get chef node data from chef server
    $chef_node = shop_get_server($server_fqdn);
  
  }
  // UPDATE: If a server was found, we will update it's attributes
  else {
    
    drush_log("[DEVUDO] Chef node found with name:$server_fqdn " . $chef_node->automatic->ipaddress, 'ok');
    
    // Save new json data to file
    $attributes_json = json_encode($attributes);
    file_put_contents($json_path, $attributes_json);
    
    // Sync file to server
    // Use IP in case something is wrong with DNS
    if (!empty($chef_node->automatic->ipaddress)){
      $host = $chef_node->automatic->ipaddress;
    }
    else {
      $host = $server_fqdn;
    }
    
    // @TODO: This line implies that aegir already has ssh access to devudo@host
    shop_exec("scp $json_path devudo@$host:~/attributes.json");

    // Run chef-client to update the server itself.
    $chef_client_cmd = "sudo /usr/bin/chef-client -j attributes.json";
    $chef_client_cmd_exec = escapeshellarg($chef_client_cmd);
    drush_log("[DEVUDO] Running chef-client on $server_fqdn:", 'notice');
    shop_exec_interactive("knife ssh name:$server_fqdn -x devudo $chef_client_cmd_exec -a ipaddress");
  }
  
  // EVERYTHING BELOW HAPPENS WHETHER THE SERVER IS NEWLY CREATED OR NOT.
  
  // If we have the IP, save it here
  // @TODO: Should we grab both public and private IPs?
  $ips = array();
  if (!empty($chef_node->automatic->ipaddress)){
    drush_log("[DEVUDO] IP address for $server_fqdn loaded from chief.devudo.com: $chef_node->automatic->ipaddress", 'ok');
    
    $ips[] = $chef_node->automatic->ipaddress;
    $task->ref->ip_addresses[] = array($chef_node->automatic->ipaddress);
    
    
  }
  // $ipaddress is returned from the server create process.  If we are testing,
  // $chef_node might not exist, so this is here mostly for debugging or if something
  // goes wrong when connecting to the chef server, we might already have the IP
  elseif (isset($ipaddress)) {
    drush_log("[DEVUDO] IP address for $server_fqdn NOT FOUND from chief.devudo.com, but we got it from the knife rackspace server create: $ipaddress", 'ok');
    $ips[] = $ipaddress;
  }

  
  // if we got IPs, save them now.
  if (!empty($ips)){
    $task->ref->ip_addresses = $ips;
    
    // @TODO: SAVE DNS!!! (But not here!) This will be much slower here...
    // We need to add the DNS record the moment that rackspace
    // grants an IP address for this server.  A Hosting queue is probably our best bet.
    
  }
  
  // if we got a rackspace ID, save it now.
  if (!empty($ips)){
    $task->ref->ip_addresses = $ips;
  }
    
  
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
  $task = &drush_get_context('HOSTING_TASK');
  
  // Get some vars and attributes
  $server_fqdn = $task->ref->title;
  $attributes = $task->ref->attributes;
  $server_id = $task->ref->rackspace_id;
  
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
  if ($last_line[2] == "WARNING: Deleted server $server_id") {
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
 * exec interactive
 */
function shop_exec_interactive($command, $label = "EXEC"){
  drush_log("[$label] Running: $command");
  _drush_shell_exec(array($command), TRUE);
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
  // drush_log("[$label] Returned: $json");
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
  $role = ''; //$attributes->devudo['chef_role'];
  $rackspace_flavor = $attributes->devudo['rackspace_flavor'];

  // If no rackspace flavor it is doomed
  if (!$rackspace_flavor) {
    return drush_log(dt('No rackspace_flavor found!'), 'error');
  }
  
  $attributes_json = json_encode($attributes);

  drush_set_option('rackspace_flavor', $rackspace_flavor);
  drush_set_option('attributes', $attributes_json);
  $data = drush_shop_provision_server_create($server_fqdn, $role);
  
  return $data;
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
