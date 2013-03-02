<?php

/**
 * Implements drush_HOOK_pre_COMMAND()
 *
 * This runs for each tasks during the command
 *    drush @hostmaster hosting-tasks
 *
 * NOTE: This ONLY runs when being called from a hostmaster task.
 */
function drush_shop_provision_pre_hosting_task() {
  // Get Hostmaster Task object
  $task =& drush_get_context('HOSTING_TASK');
  // NOT READY YET
  //// On server delete, run the commands 
  //if ($task->ref->type == 'server' && $task->task_type == 'delete') {
  //  drush_log("[DEVUDO SHOPMASTER] DELETINGSERVER");
  //  $server_fqdn = $task->ref->title;
  //  $server_id = $task->rackspace_server_id;
  //  $cmd = "knife rackspace server delete $server_id -P";
  //  drush_log("[DEVUDO SHOPMASTER] Running: $cmd");
  //  drush_shell_exec($cmd);
  //  $lines = drush_shell_exec_output();
  //  foreach ($lines as $line){
  //    drush_log("[KNIFE] $line");
  //  }
  //
  //}
  
  // On server verify, save new attributes to this node.
  if ($task->ref->type == 'server' && $task->task_type == 'verify') {
    $server_fqdn = $task->ref->title;
    
    // If it has been created already:
    $server_node_data = shop_exec_json("knife node show $server_fqdn -a ipaddress -Fj");
    
    if (empty($server_node_data)){
      drush_log("[DEVUDO] IP Address NOT FOUND: Creating server");
      
      // CREATE SERVER
      shop_exec("knife rackspace server create -r 'role[devmaster]' --server-name $server_fqdn --image 5cebb13a-f783-4f8c-8058-c4182c724ccd --flavor 2 -Fjson");

      // Get IP Address
      $server_node_data = shop_get_server($server_fqdn);
      shop_log_object($server_node_data, '$server_node_data');
      drush_log("[DEVUDO SHOPMASTER] SERVER CREATED! IP Address GRABBED: $server_node_data->ipaddress");
      $task->ref->ip_addresses[] = $server_node_data->ipaddress;
    } else {
      drush_log("[DEVUDO] Chef Server Node found for $server_fqdn");
    }
    
    $attributes = $task->ref->attributes;
    $json_path = "/tmp/$server_fqdn.json";
    
    drush_log("[DEVUDO] Adding attributes to $server_fqdn",'notice');
    
    // Get chef node data
    $chef_node = shop_get_server($server_fqdn);

    // Alter chef normal attributes
    $chef_node->normal = $attributes;
    shop_log_object($chef_node, $json_path);
    
    // Save new json data to file
    $chef_node_string = json_encode($chef_node);
    file_put_contents($json_path, $chef_node_string);
    //drush_register_file_for_deletion($json_path);
    
    // Save chef node (on chef server) from file.
    shop_exec("knife node from file $json_path");
    
    //HERE WE GO!
    shop_exec("knife ssh name:$server_fqdn -x devudo 'sudo chef-client' -a ipaddress");
    
  }
}

/**
 * Helpful Exec
 */
function shop_exec($command, $label = "EXEC", $command_label = "KNIFE"){
  drush_log("[$label] Running: $command");
  drush_shell_exec($command);
  $lines = drush_shell_exec_output();
  foreach ($lines as $line){
    drush_log("[$command_label] $line");
  }
  return implode("\n", $lines);
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
function shop_get_server($node_name){
  return shop_exec_json("knife node show $node_name -Fj");
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