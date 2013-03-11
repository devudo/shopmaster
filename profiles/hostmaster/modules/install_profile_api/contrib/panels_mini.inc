<?php

/**
 * Import a mini panel export from a file.
 *
 * @param $file
 *   The file to import.
 * @param $title
 *   Optional, override the title of the mini panel.
 * @param $name
 *   Optional, override the name of the mini panel
 * @param $category
 *   Optional, override the category of the mini panel
 **/
function install_panels_mini_import_from_file($file, $title = '', $name = '', $category = '') {
  if (!file_exists($file)) {
    drupal_set_message(t("Could not find mini panels export %file", array('%file' => $file)));
    return FALSE;
  }

  module_load_include('inc', 'panels_mini', 'panels_mini.admin');

  $export = file_get_contents($file);
  ob_start();
  eval($export);
  ob_end_clean();

  if (!isset($mini)) {
    drupal_set_message(t("Could not load mini panels export %file", array('%file' => $file)));
    return FALSE;
  }

  $form_state = array();
  $form_state['values']['pid'] = $mini->pid;
  $form_state['values']['title'] = $title ? $title : $mini->title;
  $form_state['values']['name'] = $name ? $name : $mini->name;
  $form_state['values']['category'] = $category ? $category : $mini->category;
  $form_state['values']['op'] = t('Save');

  drupal_execute('panels_mini_edit_form', $form_state, $mini);

  // Turn form errors into visible error messages
  $errors = form_set_error(NULL, '', TRUE);
  foreach ($errors as $field => $message) {
    drupal_set_message($message, 'error');
  }
}
