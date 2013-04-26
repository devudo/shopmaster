<?php

define('DRUSH_DEVUDO_ERROR', 10);


/**
 * Base class for dealing with knife
 */
class provisionService_devudo_servers extends provisionService_devudo {
  
  
  /**
   * This is run immediately after provision loads "servers driver for devudo service"
   *
   * Also it runs many times, for some reason.
   */
  function init_server() {
    //drush_log('[DEVUDO] provisionService_devudo_servers::init_server()', 'ok');
    parent::init_server();
    
    // Get war path and solr_homes_path
    $this->server->rackspace_image = drush_get_option('rackspace_image', '');
    $this->server->rackspace_flavor = drush_get_option('rackspace_flavor', '');
    $this->server->attributes_json = drush_get_option('attributes_json', '');
    $this->server->role = drush_get_option('role', '');
    
//    $this->server->rackspace_id = '';
  }
  
  /**
   * This is run immeidately after provision saves the server config.
   * (near the end)
   */
  function verify_server_cmd() {
    drush_log('[DEVUDO] Verifying Server ' . d()->remote_host, 'ok');
      
    $server_fqdn = d()->remote_host;
    $role = $this->server->role;
   
    // @TODO: Save these to the context/drush alias
    $rackspace_flavor = $this->server->rackspace_flavor;  // 2
    $rackspace_image = $this->server->rackspace_image;  //e4dbdba7-b2a4-4ee5-8e8f-4595b6d694ce
    $attributes = $this->server->attributes_json;
    
    // Look for this chef node on Chef Server
    drush_log("[DEVUDO] Looking for chef node $server_fqdn on chef server", 'ok');
    $chef_node = shop_get_server($server_fqdn);
    
    // If shop_get_server() returns a string, knife node show didn't work.
    if (is_string($chef_node)) {
      
      // If the error is NOT object not found, there was a more serious error
      if (strpos($chef_node, 'ERROR: The object you are looking for could not be found') !== 0){
        return drush_set_error(DRUSH_DEVUDO_ERROR, '[DEVUDO] knife failed: ' . $chef_node);
      }
      // Otherwise, we just don't have a chef node of that name yet.
      // So, create a new server.
      drush_log("[DEVUDO] Chef Node not found. Creating server...", 'ok');
      
      drush_log("[DEVUDO] Running: drush server-create $server_fqdn $role --rackspace_flavor=$rackspace_flavor --rackspace_image=$rackspace_image", 'ok'); 

      drush_set_option('rackspace_flavor', $rackspace_flavor);
      drush_set_option('rackspace_image', $rackspace_image);
      drush_set_option('role', $role);
        
////  $attributes_json = json_encode($attributes);
////  drush_set_option('attributes', $attributes_json);
      //$data = drush_shop_provision_server_create($server_fqdn);
      
      shop_log_object($data, 'data from server create');

    }
    // If we got a server node...
    else {
      $ip = $chef_node->automatic->ipaddress;
      drush_log("[DEVUDO] Chef node found with name:$server_fqdn $ip Preparing attributes...", 'ok');
    
    }
    
    //
    //shop_exec('knife node list');
    parent::verify_server_cmd();
  }
  
  
  
  function config_data($config = null, $class = null) {
    $data = parent::config_data($config, $class);
    $data['rackspace_image'] = $this->server->rackspace_image;
    $data['rackspace_flavor'] = $this->server->rackspace_flavor;
    $data['role'] = $this->server->role;
    $data['attributes_json'] = $this->server->attributes_json;
    return $data;
  }
  
  static function option_documentation() {
    return array(
      '--rackspace_image' => 'The rackspace server image.',
      '--rackspace_flavor' => 'The rackspace server flavor.',
      '--role' => 'The chef role.',
      '--attributes_json' => 'JSON encoded attributes.',
    );
  }
  
  /**
   * Ask the web server to check for and load configuration changes.
   */
  function parse_configs() {
    return TRUE;
  }
}
