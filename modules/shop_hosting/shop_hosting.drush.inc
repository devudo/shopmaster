<?php

/**
 * hook_post_hosting_TASK_TYPE_task($task, $data)
 */
function shop_hosting_post_hosting_verify_task($task, $data){
  if ($task->ref->type == server){
    shop_provision_verify_server();
  }
}

/**
 * hook_post_hosting_TASK_TYPE_task($task, $data)
 * for delete tasks.
 */
function shop_hosting_post_hosting_delete_task($task, $data){
  // DESTROY the server...
  if ($task->ref->type == server && shop_provision_delete_server()){
    // Unpublish the node.
    $task->ref->status = 0;
    $task->ref->no_verify = TRUE;
    node_save($task->ref);
  }
}