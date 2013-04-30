<?php

/**
 * Implements hook_hosting_ENTITYTYPE_context_options() for sites.
 */
function shop_hosting_hosting_server_context_options(&$task) {
  $task->context_options['rackspace_id'] = $task->ref->rackspace_id;
  $task->context_options['rackspace_image'] = $task->ref->attributes->devudo['rackspace_image'];
  $task->context_options['rackspace_flavor'] = $task->ref->attributes->devudo['rackspace_flavor'];
  $task->context_options['role'] = $task->ref->attributes->devudo['chef_role'];
  $task->context_options['attributes_json'] = json_encode($task->ref->attributes);
}

/**
 * Implementation of hook_drush_context_import().
 */
function hosting_solr_drush_context_import($context, &$node) {
  if ($context->type == 'server') {
    if (isset($context->rackspace_flavor)) {
      $node->field_rackspace_flavor[0]['value'] = $context->rackspace_flavor;
      $node->field_chef_role[0]['value'] = $context->role;
    }
  }
}

/**
 * Only runs when it works...
 */
function shop_hosting_post_hosting_verify_task($task, $data){
  
  if ($data['context']['rackspace_id']){
    $task->ref->rackspace_id = $data['context']['rackspace_id'];
  }
  
  $task->ref->ip_addresses = $data['context']['ip_addresses'];
  
  drush_log("Trying to save node with $task->ref->rackspace_id $task->ref->ip_addresses", 'ok');
  node_save($task->ref);
}

/**
 * Only runs when it works...
 */
function shop_hosting_post_hosting_delete_task($task, $data){
  // DESTROY the server...
  if ($task->ref->type == 'server'){
    // Unpublish the node.
    $task->ref->status = 0;
    $task->ref->no_verify = TRUE;
    //node_save($task->ref);
  }
}
