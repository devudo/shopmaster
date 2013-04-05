<?php

/**
 * Respond to creation of a new SSH key.
 *
 * This hook is invoked from sshkey_save() after the SSH key is inserted into
 * the sshkey table in the database.
 *
 * @param $key
 *   The SSH key object that was created.
 */
function shop_hosting_sshkey_insert($key) {
  shop_hosting_verify_servers_with_key($key);
}

/**
 * Respond to updates to an SSH key.
 *
 * This hook is invoked from sshkey_save() after the SSH key is updated in the
 * sshkey table in the database.
 *
 * @param $key
 *   The SSH key object that was updated.
 */
function shop_hosting_sshkey_update($key) {
  shop_hosting_verify_servers_with_key($key);
}

/**
 * Respond to an SSH key deletion.
 *
 * This hook is invoked from sshkey_delete_multiple() after the SSH key has
 * been removed from the sshkey table in the database.
 *
 * @param $key
 *   The SSH key object that was deleted.
 */
function shop_hosting_sshkey_delete($key) {
  shop_hosting_verify_servers_with_key($key);
}

/**
 * Helper to save verify tasks for all servers a user has access to.
 */
function shop_hosting_verify_servers_with_key($key){
  if ($key->entity_type == 'user'){
    $servers = shop_hosting_get_servers($key->entity_id);
    foreach ($servers as $nid => $name){
      hosting_add_task($nid, 'verify');
    }
    drupal_set_message(t('The @server %list will be updated shortly.', array('@server' => format_plural(count($servers), 'server', 'servers'), '%list' => implode(', ', $servers))));
  }
}

/**
 * Get servers that a user has access to.
 */
function shop_hosting_get_servers($uid) {
  $account = user_load($uid);
  $servers = array();

  // add all servers if user can access all servers
  if (user_access('access all servers', $account)){
    $results = db_query('SELECT nid, title FROM node WHERE type = "server" AND status = 1');
    while ($node = db_fetch_object($results)){
      $servers[$node->nid] = $node->title;
    }
  }
  
  // find CCK userreference field tables
  // search through them for matching user ids and load those nodes
  $types = content_types();

  // Find the table and columns to search through, if the same
  // table comes up in more than one content type, we only need
  // to search it once.
  $search_tables = array();
  foreach ($types as $type_name => $type) {
    foreach ($type['fields'] as $field) {
      // Only add tables when reverse link has been selected.
      if ($field['type'] == 'userreference' && !empty($field['widget']['reverse_link'])) {
        $db_info = content_database_info($field);
        $search_tables[$db_info['table']][] = $db_info['columns']['uid']['column'];
      }
    }
  }
  foreach ($search_tables as $table => $columns) {
    foreach ($columns as $column) {
      $ids = db_query(db_rewrite_sql("SELECT DISTINCT(n.nid), n.title, n.type FROM {node} n LEFT JOIN {". $table ."} f ON n.vid = f.vid WHERE f.". $column ."=". $account->uid. " AND n.status = 1"));
      while ($data = db_fetch_object($ids)) {
        if ($data->type == 'server'){
          $servers[$data->nid] = $data->title;
        }
      }
    }
  }
  return $servers;
}