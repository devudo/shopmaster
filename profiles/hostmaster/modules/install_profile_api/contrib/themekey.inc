<?php


/**
 * Add a new themekey path.
 * 
 * @param $path
 *   Drupal path where this theme should be used.
 * @param $theme
 *   The theme to switch to.
 */
function install_themekey_add_path($path, $theme, $fit = 1, $weight = 0, $wildcards = array(), $conditions = array(), $custom = 1, $callbacks = array()) {
  db_query(
    "INSERT INTO themekey_paths
     (path, fit, weight, wildcards, conditions, custom, theme, callbacks)
     VALUES
     ('%s', %d, %d, '%s', '%s', %d, '%s', '%s')",
    $path, $fit, $weight, serialize($wildcards), serialize($conditions), $custom, $theme, serialize($callbacks)
  );
}
