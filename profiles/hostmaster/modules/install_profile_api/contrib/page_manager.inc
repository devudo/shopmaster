<?php

/**
 * Import a custom page from a file.
 *
 * @param $file
 *   The file to import.
 * @param $name
 *   Optional, override the name of the page.
 * @param $path
 *   Optional, override the path of the page.
 * @param $overwrite
 *   If a page of the same name is found, overwrite it with this import.  By
 *   default pages will not overwrite each other.
 **/
function install_page_manager_import_from_file($file, $name = '', $path = '', $overwrite = FALSE) {
  if (!file_exists($file)) {
    drupal_set_message(t("Could not find page_manager export %file", array('%file' => $file)));
    return FALSE;
  }

  module_load_include('inc', 'page_manager', 'plugins/tasks/page.admin');
  module_load_include('inc', 'page_manager', 'page_manager.admin');

  $export = file_get_contents($file);

  $form_state = array();
  $form_state['values']['name'] = $name;
  $form_state['values']['path'] = $path;
  $form_state['values']['overwrite'] = $overwrite;
  $form_state['values']['object'] = $export;
  $form_state['values']['op'] = t('Import');
  $form = page_manager_page_import_subtask($form_state, 'page');

  // Not using drupal_execute because it doesn't pass $form_state by
  // reference, specifically the 'cache' element is lost.
  page_manager_page_import_subtask_validate($form, $form_state);
  page_manager_page_import_subtask_submit($form, $form_state);

  // Turn form errors into visible error messages
  $errors = form_set_error(NULL, '', TRUE);
  foreach ($errors as $field => $message) {
    drupal_set_message($message, 'error');
  }

  // Save the cached page
  if (!$errors) {
    $task_name = page_manager_make_task_name('page', $form_state['values']['name']);
    $cache = page_manager_get_page_cache($task_name);
    page_manager_save_page_cache($cache);
  }
}
