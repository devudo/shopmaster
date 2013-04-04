<?php

/**
 * Implementation of hook_user_default_permissions().
 */
function shop_servers_user_default_permissions() {
  $permissions = array();

  // Exported permission: create server
  $permissions['create server'] = array(
    'name' => 'create server',
    'roles' => array(
      '0' => 'administrator',
    ),
  );

  // Exported permission: delete server
  $permissions['delete server'] = array(
    'name' => 'delete server',
    'roles' => array(
      '0' => 'administrator',
    ),
  );

  // Exported permission: edit server
  $permissions['edit server'] = array(
    'name' => 'edit server',
    'roles' => array(
      '0' => 'administrator',
    ),
  );

  // Exported permission: view server
  $permissions['view server'] = array(
    'name' => 'view server',
    'roles' => array(
      '0' => 'administrator',
    ),
  );

  return $permissions;
}
