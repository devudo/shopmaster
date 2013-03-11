<?php

/**
 * @file
 * Provide simpler functions to create webform fields
 */

/**
 * Generic function to add a new component to a webform
 */
function install_webform_add_component($nid, $cid, $pid, $name, $key, $type, $default='', $mandatory=1, $email=1, $weight=0, $attributes=array()) {
  $extras = array_merge(array('email' => 0, 'disabled' => 0), $attributes);
  db_query("INSERT INTO {webform_component} (nid, cid, pid, form_key, name, type, value, extra, mandatory, email, weight) VALUES (%d, %d, %d, '%s', '%s', '%s', '%s', '%s', %d, %d, %d)", $nid, $cid, $pid, $key, $name, $type, $value, serialize($extras), $mandatory, $email, $weight);
  return;
}

/**
 * Simpler function for adding email components
 */
function install_webform_add_email($nid, $cid, $pid, $name, $key, $default='', $mandatory=1, $email=1, $weight=0, $attributes=array()) {
  return install_webform_add_component($nid, $cid, $pid, $name, $key, 'email', $default='', $mandatory=1, $email=1, $weight=0, $attributes);
}
