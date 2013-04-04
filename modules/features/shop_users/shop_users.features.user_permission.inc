<?php

/**
 * Implementation of hook_user_default_permissions().
 */
function shop_users_user_default_permissions() {
  $permissions = array();

  // Exported permission: manage own SSH public keys
  $permissions['manage own SSH public keys'] = array(
    'name' => 'manage own SSH public keys',
    'roles' => array(
      '0' => 'administrator',
      '1' => 'authenticated user',
    ),
  );

  // Exported permission: view own SSH public keys
  $permissions['view own SSH public keys'] = array(
    'name' => 'view own SSH public keys',
    'roles' => array(
      '0' => 'administrator',
      '1' => 'authenticated user',
    ),
  );

  return $permissions;
}
