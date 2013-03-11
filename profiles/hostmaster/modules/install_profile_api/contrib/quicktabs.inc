<?php

/**
 * Creates a new Quick Tabs block.
 *
 * @param $title
 *   Title of the Quick Tabs block
 * @param $tabs
 *   An array of tab data
 * @param $ajax
 *   TRUE or FALSE, should this Quick Tabs block use AJAX?  Defaults to FALSE.
 * @param $style
 *   What style should this Quick Tabs block use?  Defaults to 'default'.
 */
function install_quicktabs_add($title, $tabs, $ajax = FALSE, $style = 'default') {
  if (!$title) {
    return FALSE;
  }
  else if (!is_array($tabs) || !count($tabs)) {
    return FALSE;
  }
  else if (!$style) {
    return FALSE;
  }

  db_query("INSERT INTO {quicktabs} (ajax, title, tabs, style) VALUES (%d, '%s', '%s', '%s')", $ajax, $title, serialize($tabs), $style);

  $qtid = db_last_insert_id('quicktabs', 'qtid');

  return $qtid;
}
