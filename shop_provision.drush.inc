<?php

/**
 * Implements drush_HOOK_pre_COMMAND()
 *
 * This runs for each tasks during the command
 *    drush @hostmaster hosting-tasks
 *
 * NOTE: This ONLY runs when being called from a hostmaster task.
 * This hook should ONLY be used to pass Options from a hostmaster task form to
 * the $task object, or if you don't need this functionality from the command
 * line.
 */
function drush_shop_provision_pre_hosting_task() {
  // Get Hostmaster Task object
  $task =& drush_get_context('HOSTING_TASK');
  
  // On server verify, save new attributes to this node.
  if ($task->ref->type == 'server' && $task->task_type == 'verify') {
    
    $server = $task->ref->title;
    $attributes = $task->ref->attributes;
    $json_path = "/tmp/$server.json";
    
    drush_log("[DEVUDO SHOPMASTER] Adding attributes to $server",'notice');
    
    // Get chef node data
    drush_shell_exec("knife node show $server -l -Fj");
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
    
  }
}
