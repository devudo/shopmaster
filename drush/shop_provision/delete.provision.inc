<?php

function drush_devudo_provision_delete() {
  if (d()->type === 'server') {
    
    // run knife rackspace server create.
    $rackspace_id = d()->rackspace_id;
    
    shop_log_object(d()->server);
    
    if ($rackspace_id){
      $cmd =  "knife rackspace server delete $rackspace_id -P -y";
      $lines = shop_exec_return($cmd);
      shop_log_object($lines, 'return from server delete');
      return TRUE;

    } else {
      return drush_set_error(DRUSH_FRAMEWORK_ERROR, dt('No rackspace ID found.  Unable to delete server.'));
    }
    
  }
}