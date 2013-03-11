<?php

/**
 * @file context.inc
 *  Enables contexts to be imported from flat files.
 */

/**
 * Import a context from a flat file.
 *
 * This method of importing contexts is useful because it will override
 * context that are stored "in code" by other modules.
 *
 * @param $file
 *   The path to a file containing the export.
 */
function install_context_import_from_file($file) {
  ob_start();
  include $file;
  ob_end_clean();

  if (!isset($context) || !$context) {
    drupal_set_message(t("Could not load context export %file", array('%file' => $file)));
    return FALSE;
  }

  if (is_array($context)) {
    $context = (object) $context;
  }

  // Enable this context to update an existing context in the same
  // namespace > section > value.
  $existing = context_load_context($context);
  if ($existing != FALSE && $existing->cid) {
    $context->cid = $existing->cid;
  }

  context_save_context($context);

  return $context;
}

