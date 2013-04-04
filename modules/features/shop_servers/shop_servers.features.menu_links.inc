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
    'router_path' => 'node/add',
    'link_title' => 'Create a Server',
    'options' => array(
      'attributes' => array(
        'title' => '<strong>The physical machine which will provide various services to your site.</strong>
                Each server can have multiple services associated to it. To host a site you require a database service and a web service, which can either be provided by a single or multiple servers.',
      ),
    ),
    'module' => 'system',
    'hidden' => '0',
    'external' => '0',
    'has_children' => '0',
    'expanded' => '0',
    'weight' => '-50',
  );
  // Exported menu link: primary-links:servers
  $menu_links['primary-links:servers'] = array(
    'menu_name' => 'primary-links',
    'link_path' => 'servers',
    'router_path' => 'servers',
    'link_title' => 'Servers',
    'options' => array(),
    'module' => 'system',
    'hidden' => '0',
    'external' => '0',
    'has_children' => '0',
    'expanded' => '0',
    'weight' => '0',
  );
  // Translatables
  // Included for use with string extractors like potx.
  t('Create a Server');
  t('Servers');


  return $menu_links;
}
