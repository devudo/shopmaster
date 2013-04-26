<?php

/**
 * Base class for dealing with knife
 */
class provisionService_devudo extends provisionService {
  public $service = 'devudo';
  
  function verify_server_cmd() {
    $this->create_config($this->context->type);
    $this->parse_configs();
  }
}