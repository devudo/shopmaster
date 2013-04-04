<?php

/**
 * Implementation of hook_content_default_fields().
 */
function shop_servers_content_default_fields() {
  $fields = array();

  // Exported field: field_chef_role
  $fields['server-field_chef_role'] = array(
    'field_name' => 'field_chef_role',
    'type_name' => 'server',
    'display_settings' => array(
      'label' => array(
        'format' => 'above',
        'exclude' => 0,
      ),
      'teaser' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
      'full' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
      '4' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
    ),
    'widget_active' => '1',
    'type' => 'text',
    'required' => '1',
    'multiple' => '0',
    'module' => 'text',
    'active' => '1',
    'text_processing' => '0',
    'max_length' => '',
    'allowed_values' => '|None
devmaster|role[devmaster] Devudo DevShop server
livemaster|role[livemaster] Devudo Live Server
shopmaster|role[shopmaster] Devudo Shopmaster (This server)',
    'allowed_values_php' => '',
    'widget' => array(
      'default_value' => array(
        '0' => array(
          'value' => '',
        ),
      ),
      'default_value_php' => NULL,
      'label' => 'Role',
      'weight' => '31',
      'description' => 'Select the chef role to apply to this server.  
<p>
<em>NOTE: Don\'t change this! Only use it when creating a server.</em>
</p>',
      'type' => 'optionwidgets_buttons',
      'module' => 'optionwidgets',
    ),
  );

  // Exported field: field_parent_devmaster
  $fields['server-field_parent_devmaster'] = array(
    'field_name' => 'field_parent_devmaster',
    'type_name' => 'server',
    'display_settings' => array(
      'label' => array(
        'format' => 'above',
        'exclude' => 0,
      ),
      'teaser' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
      'full' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
      '4' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
    ),
    'widget_active' => '1',
    'type' => 'nodereference',
    'required' => '0',
    'multiple' => '0',
    'module' => 'nodereference',
    'active' => '1',
    'referenceable_types' => array(
      'server' => 'server',
      'client' => 0,
      'package' => 0,
      'task' => 0,
    ),
    'advanced_view' => '--',
    'advanced_view_args' => '',
    'widget' => array(
      'autocomplete_match' => 'contains',
      'size' => 60,
      'default_value' => array(
        '0' => array(
          'nid' => '',
        ),
      ),
      'default_value_php' => NULL,
      'label' => 'Devmaster Parent',
      'weight' => '32',
      'description' => 'ONLY if this is a live server, choose the devmaster server to attach it to.',
      'type' => 'nodereference_select',
      'module' => 'nodereference',
    ),
  );

  // Exported field: field_rackspace_flavor
  $fields['server-field_rackspace_flavor'] = array(
    'field_name' => 'field_rackspace_flavor',
    'type_name' => 'server',
    'display_settings' => array(
      'label' => array(
        'format' => 'above',
        'exclude' => 0,
      ),
      'teaser' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
      'full' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
      '4' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
    ),
    'widget_active' => '1',
    'type' => 'number_integer',
    'required' => '1',
    'multiple' => '0',
    'module' => 'number',
    'active' => '1',
    'prefix' => '',
    'suffix' => '',
    'min' => '',
    'max' => '',
    'allowed_values' => '2|512MB Standard Instance 20 GB HDD  (NOT recommended.. yet)
3|1GB Standard Instance 40 GB
4|2GB Standard Instance 80 GB  
5|4GB Standard Instance 160 GB 
6|8GB Standard Instance 320 GB 
7|15GB Standard Instance 620 GB 
8|30GB Standard Instance 1200 GB
',
    'allowed_values_php' => '',
    'widget' => array(
      'default_value' => array(
        '0' => array(
          'value' => '',
        ),
      ),
      'default_value_php' => NULL,
      'label' => 'Server Size',
      'weight' => '33',
      'description' => 'Choose the size of this server.',
      'type' => 'optionwidgets_buttons',
      'module' => 'optionwidgets',
    ),
  );

  // Exported field: field_server_users
  $fields['server-field_server_users'] = array(
    'field_name' => 'field_server_users',
    'type_name' => 'server',
    'display_settings' => array(
      'label' => array(
        'format' => 'above',
        'exclude' => 0,
      ),
      'teaser' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
      'full' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
      '4' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
    ),
    'widget_active' => '1',
    'type' => 'userreference',
    'required' => '0',
    'multiple' => '1',
    'module' => 'userreference',
    'active' => '1',
    'referenceable_roles' => array(
      '2' => 0,
      '5' => 0,
      '4' => 0,
      '3' => 0,
    ),
    'referenceable_status' => '1',
    'advanced_view' => '',
    'advanced_view_args' => '',
    'widget' => array(
      'autocomplete_match' => 'contains',
      'size' => '60',
      'reverse_link' => 1,
      'default_value' => array(
        '0' => array(
          'uid' => NULL,
          '_error_element' => 'default_value_widget][field_server_users][0][uid][uid',
        ),
      ),
      'default_value_php' => NULL,
      'label' => 'Users',
      'weight' => '34',
      'description' => 'You may grant extra users access to your server here.  All "administrator" users will automatically be granted access to all servers.

Each user will get a linux user created and their public SSH keys uploaded to that user, and to the "aegir" user on this server.',
      'type' => 'userreference_autocomplete',
      'module' => 'userreference',
    ),
  );

  // Translatables
  // Included for use with string extractors like potx.
  t('Devmaster Parent');
  t('Role');
  t('Server Size');
  t('Users');

  return $fields;
}
