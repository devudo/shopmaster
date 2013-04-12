<?php

/**
 * Implementation of hook_menu_default_menu_links().
 */
function shop_servers_menu_default_menu_links() {
  $menu_links = array();

  // Exported menu link: primary-links:node/add/server
  $menu_links['primary-links:node/add/server'] = array(
    'menu_name' => 'primary-links',
    'link_path' => 'node/add/server',
    'router_path' => 'node/add/server',
    'link_title' => 'Create a Server',
    'options' => array(
      'attributes' => array(
        'title' => '',
      ),
    ),
    'module' => 'menu',
    'hidden' => '0',
    'external' => '0',
    'has_children' => '0',
    'expanded' => '0',
    'weight' => '0',
  );
  // Translatables
  // Included for use with string extractors like potx.
  t('Create a Server');


  return $menu_links;
}
