<?php

/**
 * Import a view from a file.
 *
 * @param $file
 *   The path to a file containing the export.
 * @param $name
 *   The name of the view, must be EXACTLY the same as the 'name'
 *   attribute in the export!
 */
function install_views_ui_import_from_file($file, $name = '') {
  if (!file_exists($file)) {
    return FALSE;
  }

  $export = file_get_contents($file);

  // Strip any leading php tags
  $export = preg_replace('/^<\?(php)?/', '', $export);

  views_include('view');

  ob_start();
  eval($export);
  ob_end_clean();

  if (!isset($view) || !is_object($view) || !$view->name) {
    drupal_set_message(t("Could not load view export %file", array('%file' => $file)));
    return FALSE;
  }

  // TODO: Let views validate the $name?
  if ($name) {
    $view->name = $name;
  }

  // Ensure the views cache is populated, in case the import is meant to
  // override an existing view.  If a view of this name doesn't exist
  // views_ui_cache_load() will gracefully fail.
  $existing_view = views_ui_cache_load($view->name);

  $view->save();
  views_object_cache_clear('view', $view->name);
}

/**
 * Disables a view by name.
 * 
 * @param $name
 *  The name of the view to disable.
 * @param $rebuild_menu
 *  The menu tree must be rebuilt after install_views_disable_view() is
 *  called, however when called many times in a row it may be sensible
 *  to only rebuild the menu tree once at the end.
 **/
function install_views_disable_view($name, $rebuild_menu = TRUE) {
  $views_status = variable_get('views_defaults', array());
  $views_status[$name] = TRUE; // True is disabled
  variable_set('views_defaults', $views_status);
  views_invalidate_cache();

  if ($rebuild_menu) {
    menu_rebuild();
  }
}
