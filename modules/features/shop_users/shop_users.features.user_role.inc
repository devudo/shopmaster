<?php

/**
 * Implementation of hook_user_default_roles().
 */
function shop_users_user_default_roles() {
  $roles = array();

  // Exported role: administrator
  $roles['administrator'] = array(
    'name' => 'administrator',
  );

  return $roles;
}
