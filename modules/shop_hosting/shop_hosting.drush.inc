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

/**
 * Passes attributes to the ServerContext.
 *
 * @see provision.context.server.inc!  We had to hack that file
 * to get attributes into the server context.
 */
function shop_hosting_hosting_server_context_options(&$task) {
  $task->context_options['attributes'] = $task->ref->attributes;
  
}