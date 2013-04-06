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
 * Implements drush_HOOK_pre_COMMAND
 *
 * $task is available.
 * d() is server.
 * PROPERTIES SET TO d() here are persisted to the other "task" hooks, but NOT
 * to the provision-XXX hooks. Basically, NO drush contexts are persisted from here
 * to the provision-XXX hooks!!!
 *
 */
function drush_shop_hosting_pre_hosting_task() {
  $task = &drush_get_context('HOSTING_TASK');
  if ($task->ref->type == 'server') {
    
    if ($task->task_type == 'verify'){
      // Create or update the server.
      shop_provision_verify_server();
      
    } elseif ($task->task_type == 'delete'){
      // DESTROY the server.
      shop_provision_delete_server();
    }
  }
}

/**
 * Implements hook_post_hosting_TASKTYPE_task()
 *
 * ONLY runs when a task does not fail.
 * 
 * Save data from rackspace to the server node.
 * 
 * $task is available.
 * d() is server. HAS PROPERTIES FROM drush_shop_hosting_pre_hosting_task()
 */
function shop_hosting_post_hosting_verify_task($task, $data) {
  if ($task->ref->type == 'server') {
    //drush_log('shop_hosting_post_hosting_verify_task()', 'warning');
    //drush_log('d() = ' . print_r(d(), 1), 'warning');
    //drush_log('$task = ' . print_r($task, 1), 'warning');
  }
}


/**
 * Implements hook_post_hosting_TASKTYPE_task()
 *
 * ONLY runs when a task FAILS.
 *
 * $task is available.
 * d() is server. HAS PROPERTIES FROM drush_shop_hosting_pre_hosting_task()
 */
function shop_hosting_hosting_verify_task_rollback($task, $data) {
  if ($task->ref->type == 'server') {
    //drush_log('shop_hosting_hosting_verify_task_rollback()', 'warning');
    //drush_log('d() = ' . print_r(d(), 1), 'warning');
    //drush_log('$task = ' . print_r($task, 1), 'warning');
  }
}


/**
 * Passes attributes to the ServerContext.
 *
 * @see provision.context.server.inc!  We had to hack that file
 * to get attributes into the server context.
 *
 * Stupid aegir... unhacked for now.
 */
function shop_hosting_hosting_server_context_options(&$task) {
  //$task->context_options['devudo'] = 'GOTCHA';
  //$task->context_options['attributes'] = $task->ref->attributes;
  //
}