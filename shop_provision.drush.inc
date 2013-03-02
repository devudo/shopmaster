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
    $command = "knife node show $server_fqdn -a ipaddress -Fj";
    drush_log("[DEVUDO] Running: $command");
    drush_shell_exec($command);
    $server_node_data_json = implode(drush_shell_exec_output());
    $server_node_data = json_decode(implode($server_node_data_json));
    
    if (!empty($server_node_data)){
      drush_log("[DEVUDO SHOPMASTER] Server Node Found: $server_node_data_json");
    } else {
      drush_log("[DEVUDO SHOPMASTER] IP Address NOT FOUND: Creating server");
      // CREATE SERVER
      $command = "knife rackspace server create -r 'role[devmaster]' --server-name $server_fqdn --image 5cebb13a-f783-4f8c-8058-c4182c724ccd --flavor 2 -Fjson";
      drush_log("[DEVUDO SHOPMASTER] Running: $command");

      drush_shell_exec($command);
      $lines = drush_shell_exec_output();
      
      foreach ($lines as $line){
        drush_log("[KNIFE] $line");
      }

      // Get Data
      drush_shell_exec("knife node show $server_fqdn -a ipaddress -Fj");
      $server_node_data = drush_shell_exec_output();
      $server_node_data = json_decode(implode($server_node_data));
      
      drush_log("[DEVUDO SHOPMASTER] SERVER CREATED! IP Address GRABBED: $server_node_data->ipaddress");
      $task->ref->ip_addresses[] = $server_node_data->ipaddress;
      $task->context_options['devudo_ip'] = $server_node_data->ipaddress;
    }
    
    $attributes = $task->ref->attributes;
    $json_path = "/tmp/$server_fqdn.json";
    
    drush_log("[DEVUDO SHOPMASTER] Adding attributes to $server_fqdn",'notice');
    
    // Get chef node data
    drush_shell_exec("knife node show $server_fqdn -l -Fj");
    $chef_node = drush_shell_exec_output();
    $chef_node = json_decode(implode($chef_node));

    // Alter chef normal attributes
    $chef_node->normal = $attributes;
    
    // Save new json data to file
    $chef_node_string = json_encode($chef_node);
    file_put_contents($json_path, $chef_node_string);
    drush_register_file_for_deletion($json_path);

    // Save chef node (on chef server) from file.
    drush_shell_exec("knife node from file $json_path");
    $message = drush_shell_exec_output();
    $message = implode($message);
    drush_log("[DEVUDO SHOPMASTER] From Chef: $message",'notice');
    
    // @TODO: Still needs chef-client to run recipes with new attributes.
    
    //HERE WE GO!
    $command = "knife ssh name:$server_fqdn -x devudo 'sudo chef-client' -a ipaddress";
    drush_log("[DEVUDO SHOPMASTER] Running: $command");
    drush_shell_exec($command);
    $lines = drush_shell_exec_output();
    
    foreach ($lines as $line){
      drush_log("[KNIFE] $line");
    }
    
  }
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