<?php

/**
 * Unpublishes the server node once you run "delete"
 */
function shop_hosting_post_hosting_delete_task($task, $data) {
  if ($task->ref->type == 'server') {
    $task->ref->status = 0;
    $task->ref->no_verify = TRUE;
    node_save($task->ref);
  }
}