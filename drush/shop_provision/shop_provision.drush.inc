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
    ),
    'options' => array(
      'attributes' => 'A JSON string of attributes.',
      'rackspace_flavor' => 'The id of the Rackspace flavor you want to use. (2 through 8)',
      'rackspace_image' => 'The image of the Rackspace flavor you want to use.',
      'role' => 'The chef role to use for this server.',
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
function drush_shop_provision_server_create($server_fqdn){
  drush_log("Hi! we're here to make a $role server called $server_fqdn!", 'ok');
  
  $rackspace_flavor = drush_get_option('rackspace_flavor', '2');
  $rackspace_image = drush_get_option('rackspace_image', '5cebb13a-f783-4f8c-8058-c4182c724ccd');
  $role = drush_get_option('role', '');

  $attributes_json = drush_get_option('attributes', '{}');
  $attributes_json = escapeshellarg($attributes_json);
  
  $run_list = ($role)? "role[$role]": "";
  $run_list = ($role)? "role[$role]": "";
  $options = ($run_list)? "-r $run_list": "";
  $options .= " --flavor $rackspace_flavor ";
  
  // Not using shpo_exec here to try and avoid these logging errors.
  $cmd = "knife rackspace server create $options --server-name $server_fqdn --image $rackspace_image -Fjson -j $attributes_json";
  
  // run knife rackspace server create.
  //$lines = shop_exec_return($cmd);
  
  /**
   * SIMULATE
   */
  //$cmd =  "knife-rackspace-create-simulate $server_fqdn $role";
  $lines = shop_exec_return($cmd);
  
    //drush_log("[EXEC] JUST KIDDING!  Moving on...", 'ok');
    //$data['Instance ID'] =  'FAke33-eee3-47ee-b52d-RACk3RIDc24b';
    //$data['Public IP Address'] =  "111.111.111.111";
    //$data['Private IP Address'] =  "111.111.111.111";
    //return $data;
    //
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
  $data_lines = array_slice($lines, -12);
  foreach ($data_lines as $line){
    drush_log($line, 'ok');
    list($key, $value) = explode(": ", $line);
    $data[$key] = $value;
  }
  
  // Make sure run list is applied.
  if ($run_list && $data['Environment'] != '_fake'){
    shop_exec("knife node run_list add $server_fqdn $run_list");
  }
  return $data;
}

/**
 * Implements drush_HOOK_pre_COMMAND
 *   for drush hosting-task
 *
 * $task is available.
 * d() is server.
 * PROPERTIES SET TO d() here are persisted to the other "task" hooks, but NOT
 * to the provision-XXX hooks. Basically, NO drush contexts are persisted from here
 * to the provision-XXX hooks!!!
 *
 * @TODO:  If we could save our attributes to the drush alias (server context) we
 * would then be able to run drush @server_NAME provison-x directly from the backend.
 *
 * Aegir sucks here.  I cannot determine how to save data to the server context.
 * We should seriously fix aegir here.  You cannot save things to the drush alias
 * (server context) without a provision service file.
 *
 * We can't fire drush_set_error() and make the task fail until this happens, and we
 * can't run provision-verify from the terminal and create a server.  Meaning, currently,
 * we can ONLY create servers from http://shop.devudo.com/node/add/server
 *
 * 
 * @NOTE!!! IF you use drush_HOOK_post_hosting_task() , the task will appear to
 * complete in the front-end, but will still continue saving entries to the task
 * log!!
 *
 * hook_post_hosting_TASK_TYPE_task($task, $data)
 *
 * 4-14: Took this away in favor of shop_hosting.drush.inc.  We must have the
 * rackspace data queue, so lets just go build that.
 
function drush_shop_provision_pre_hosting_task() {
  $task = &drush_get_context('HOSTING_TASK');
  if ($task->ref->type == 'server') {
    
    if ($task->task_type == 'verify'){
      // Create or update the server.
      //shop_provision_verify_server();
      
    } elseif ($task->task_type == 'delete'){
      // DESTROY the server.
      if (shop_provision_delete_server()){
        $task->ref->status = 0;
        $task->ref->no_verify = TRUE;
        node_save($task->ref);
      }
    }
  }
}
 */

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
// */
//function shop_provision_verify_server() {
//  $task = &drush_get_context('HOSTING_TASK');
//
//  // Get some vars and attributes
//  $server_fqdn = $task->ref->title;
//  $attributes = $task->ref->attributes;
//  $json_path = "/tmp/$server_fqdn.json";
//  
//  // Look for this chef node on Chef Server
//  drush_log("[DEVUDO] Looking for chef node $server_fqdn on chef server", 'ok');
//  $chef_node = shop_get_server($server_fqdn);
//  
//  // SOMETHING FAILED: shop_get_server() returns the string output if json didn't encode.
//  if (is_string($chef_node)) {
//    
//    // If the error is NOT object not found, there was a more serious error
//    if (strpos($chef_node, 'ERROR: The object you are looking for could not be found') !== 0){
//      drush_log('[DEVUDO] Something went wrong with knife:', 'error');
//      drush_log($chef_node, 'error');
//      return;
//    }
//    
//    // CREATE: If no server found, Create & prepare a new server
//    drush_log($chef_node, 'warning');
//    drush_log("[DEVUDO] Creating server...", 'ok');
//    
//    
//    // CREATE THE SERVER!
//    $data = shop_create_server($server_fqdn, $attributes);
//    
//    // This worked on my localhost aegir install. NOT in a virtualbox, not in a
//    // rackspace cloud... it was always causing MYSQLserver gone away.
//    
//    // One day maybe we can fix this... but in a sense, its not needed
//    // The rackspace data queue will update all of this information for us.
//
//    //$rackspace_id = $data['Instance ID'];
//    //$ipaddress =  $data['Public IP Address'];
//    
//    if (!empty($rackspace_id)){
//      drush_log("[DEVUDO] Server Created! $ipaddress  $rackspace_id", 'ok');
//      
//      // Save the Rackspace ID
//      $task->ref->rackspace_id = $rackspace_id;  
//    } else {
//      
//      // No rackspace ID? something went wrong.
//      // @TODO!  We cannot set error here because this function is
//      // being called during hosting-task, not provision-verify!
//      // If we set error now, the TASK runs FOREVER!  WTF WTF WTF
//      // return drush_set_error('DEVUDO_SERVER_CREATE', dt('Something went wrong trying to create a server on Rackspace.'));
//      return drush_log(dt('[DEVUDO] Rackspace ID not found!'), 'error');
//    }
//    
//    
//  }
//  // UPDATE: If a server was found, we will update it's attributes
//  else {
//    $ip = $chef_node->automatic->ipaddress;
//    drush_log("[DEVUDO] Chef node found with name:$server_fqdn $ip Preparing attributes...", 'ok');
//    
//    // Save new json data to file
//    $attributes_json = json_encode($attributes);
//    file_put_contents($json_path, $attributes_json);
//    
//    // Sync file to server
//    // Use IP in case something is wrong with DNS
//    if (!empty($ip)){
//      $host = $ip;
//    }
//    else {
//      $host = $server_fqdn;
//    }
//    
//    // @TODO: This line implies that aegir already has ssh access to devudo@host
//    shop_exec("scp $json_path devudo@$host:~/attributes.json");
//
//    // Run chef-client to update the server itself.
//    $chef_client_cmd = "sudo /usr/bin/chef-client -j attributes.json";
//    $chef_client_cmd_exec = escapeshellarg($chef_client_cmd);
//    drush_log("[DEVUDO] Running chef-client on $server_fqdn:", 'ok');
//    shop_exec_interactive("knife ssh name:$server_fqdn -x devudo $chef_client_cmd_exec -a ipaddress");
//  }
//  
//  // EVERYTHING BELOW HAPPENS WHETHER THE SERVER IS NEWLY CREATED OR NOT.
//  // Get chef node data from chef server
//  drush_log("[DEVUDO] Reloading server data from chef server for $server_fqdn:", 'ok');
//  $chef_node = shop_get_server($server_fqdn);
//  
//  // If we have the IP, save it here
//  // @TODO: Should we grab both public and private IPs?
//  $ips = array();
//  
//  if (!empty($chef_node->automatic->ipaddress)){
//    $ip = $chef_node->automatic->ipaddress;
//    drush_log("[DEVUDO] IP address for $server_fqdn loaded from chief.devudo.com: $ip", 'ok');
//    $ips[] = $ip;
//  }
//  // $ipaddress is returned from the server create process.  If we are testing,
//  // $chef_node might not exist, so this is here mostly for debugging or if something
//  // goes wrong when connecting to the chef server, we might already have the IP
//  elseif (isset($ipaddress)) {
//    drush_log("[DEVUDO] IP address for $server_fqdn NOT FOUND from chief.devudo.com, but we got it from the knife rackspace server create: $ipaddress", 'ok');
//    $ips[] = $ipaddress;
//  }
//  
//  
//  // if we got IPs, save them now.
//  if (!empty($ips)){
//    $task->ref->ip_addresses = $ips;
//    
//    // @TODO: SAVE DNS!!! (But not here!) This will be much slower here...
//    // We need to add the DNS record the moment that rackspace
//    // grants an IP address for this server.  A Hosting queue is probably our best bet.
//    
//  }
//  
//  // if we got a rackspace ID, save it now.
//  if (!empty($ips)){
//    $task->ref->ip_addresses = $ips;
//  }
//    
//  
//  // @TODO: If this was a livemaster server with a devmaster parent, grab the
//  // livemaster mysql root password and devmaster_parent_ip, and save it to the
//  // server context! (drush alias)
//  // Then, sync the server alias to the devmaster_parent server, then run
//  // @hostmater hosting-import "@server_NAME"!  Then, if all goes well,
//  // Devmaster will verify the server, be able to connect and create DBs, and
//  // restart NGINX!
//}
//
//
///**
// * Delete Server: $ knife rackspace server delete
// */
//function shop_provision_delete_server(){
//  $task = &drush_get_context('HOSTING_TASK');
//  
//  // Get some vars and attributes
//  $server_fqdn = $task->ref->title;
//  $attributes = $task->ref->attributes;
//  $server_id = $task->ref->rackspace_id;
//  
//  $cmd = "knife rackspace server delete $server_id -P -y";
//  drush_log("[DEVUDO] Attempting to delete server $server_fqdn, Rackspace ID: $server_id" );
//  
//  if (!$server_id) {
//    //return drush_set_error('DEVUDO_RACKSPACE_ERROR', dt('No server ID!  Without that, I cannot delete the server.'));
//    return drush_log(dt('No server ID!  Without that, I cannot delete the server.'), 'error');
//  }
//  $lines = shop_exec_return($cmd, 'RUNNING', 'KNIFE', 'lines');
//  // When successful, the last three lines look like this:
//  // [KNIFE] WARNING: Deleted server bb23f28a-c1fc-4e49-9c15-48b1807dc08c
//  // [KNIFE] WARNING: Deleted node ben.devudo.com
//  // [KNIFE] WARNING: Deleted client ben.devudo.com
//  $last_lines = array();
//  $last_line[] = array_pop($lines);
//  $last_line[] = array_pop($lines);
//  $last_line[] = array_pop($lines);
//  if ($last_line[2] == "WARNING: Deleted server $server_id") {
//    drush_log("[DEVUDO] Server Deletion successful!", 'ok');
//  } else {
//    drush_log("last line == ? " . $last_line[2], 'notice');
//    //return drush_set_error('DRUSH_FRAMEWORK_ERROR', dt('Something went wrong when trying to tell rackspace to delete the server! Please review the logs above.'));
//    drush_log(dt('Something went wrong when trying to tell rackspace to delete the server! Please review the logs above.'), 'error');
//    return FALSE;
//  }
//  // Tell drush_shop_hosting_pre_hosting_task() that we successfully deleted!
//  return TRUE;
////}
//
///*********
// * TOOLS *
// ********/
///**
// * exec interactive
// */
//function shop_exec_interactive($command, $label = "EXEC"){
//  drush_log("[$label] Running: $command");
//  _drush_shell_exec(array($command), TRUE);
//}
///**
// * Helpful Exec
// */
//function shop_exec($command, $label = "EXEC"){
//  drush_log("[$label] Running: $command", "debug");
//  drush_shell_exec($command);
//}
//
///**
// * Helpful Exec
// */
//function shop_exec_return($command, $label = "EXEC", $command_label = "KNIFE", $output = 'string'){
//  drush_log("[$label] Running: $command");
//  drush_shell_exec($command);
//  $lines = drush_shell_exec_output();
//  foreach ($lines as $line){
//    drush_log("[$command_label] $line");
//  }
//  return $output == 'string'? implode("\n", $lines): $lines;
//}
///**
// * Helpful Exec to JSon
// */
//function shop_exec_json($command, $return_string = FALSE, $label = "EXEC"){
//  drush_log("[$label] Running: $command");
//  drush_shell_exec($command);
//  $lines = drush_shell_exec_output();
//  $data_string = implode($lines);
//  $data_json = json_decode($data_string);
//  
//  // Force return string if json did not decode
//  if (!is_array($data_json) && !is_object($data_json)){
//    return $data_string;
//  } else {
//    // If asked to return a string, return a string.
//    return $return_string? $data_string: $data_json;
//  }
//  
//  
//}
//
///**
// * Helper to load a server node from chef server by name
// */
//function shop_get_server($node_name, $options = ' -l'){
//  return shop_exec_json("knife node show $node_name -Fj $options", FALSE, 'KNIFE');
//}
////
/////**
//// * Helper to create a server from scratch.  Runs knife rackspace server create 
//// */
////function shop_create_server($server_fqdn, $attributes = array()){
////  $role = $attributes->devudo['chef_role'];
////  $rackspace_flavor = $attributes->devudo['rackspace_flavor'];
////
////  // If no rackspace flavor it is doomed
////  if (!$rackspace_flavor) {
////    return drush_log(dt('No rackspace_flavor found!'), 'error');
////  }
////  
////  $attributes_json = json_encode($attributes);
////
////  drush_log("[DEVUDO] Running: drush server-create $server_fqdn $role --rackspace_flavor $rackspace_flavor --attributes=$attributes_json");
////  
////  $args[] = $server_fqdn;
////  $args[] = $role;
////  $options['rackspace_flavor'] = $rackspace_flavor;
////  $options['attributes'] = $attributes_json;
////  
////  // Invoking new process.
////  
////  //drush_invoke_process('@none', 'server-create', array($server_fqdn, $role),  array('rackspace_flavor' => $rackspace_flavor, 'attributes'=> $attributes_json));
////                                           
////  drush_set_option('rackspace_flavor', $rackspace_flavor);
////  drush_set_option('attributes', $attributes_json);
////  $data = drush_shop_provision_server_create($server_fqdn, $role);
////  return $data;
////}
//
///**
// * Helper to log an object
// */
//function shop_log_object($object, $name = '', $label = 'DEVUDO'){
//  $object = print_r($object, 1);
//  drush_log("[$label] $name => $object");
//}
