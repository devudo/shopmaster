<?php

/**
 * @file
 * Functions for manipulating nodequeues
 */


/**
 * Create a new nodequeue.
 *
 * @param $title
 *   The name of this queue.
 * @param $roles
 *   An array of integers listing the role RIDs that can add nodes to the
 *   queue. Be sure that roles you want to appear here have "manipulate
 *   queues" access in the main access control panel.
 * @param $types
 *   An array of strings listing the node type that can be added to this
 *   queue.
 * @param $size
 *   The maximum number of nodes will appear in the queue. Enter 0 for no
 *   limit (default).
 * @param $reverse
 *   Ordinarily queues are arranged with the front of the queue (where items
 *   will be removed) on top and the back (where items will be added) on the
 *   bottom. If checked, this will display the queue such that items will be
 *   added to the top and removed from the bottom.
 * @param $link
 *   If you want a link to add a node to a queue in the "links" section (next
 *   to "add new comment"), enter the text here. If left blank no link will
 *   be given; note that enabling this feature for any queue will cause an
 *   extra query to be run every time a node is loaded. "%subqueue" will be
 *   replaced with the subqueue title, if applicable.
 * @param $link_remove
 *   The text for the corresponding link to remove a node from a queue. This
 *   may be blank (in which case no link will appear) but a remove link will
 *   only appear if link, above, is set.
 * @param $subqueue_title
 *   The title for this subqueue, if applicable.
 * @param $owner
 *   By default set to 'nodequeue'.
 * @param $show_in_ui
 *   By default set to TRUE.
 * @param $show_in_tab
 *   By default set to TRUE.
 * @param $show_in_links
 *   By default set to TRUE.
 * @param $reference
 *   By default set to FALSE.
 * @param $i18n
 *   By default set to TRUE.
 *
 * @return integer
 *   The nodequeue's qid.
 */
function install_nodequeue_create($title, $roles = array(), $types = array(), $size = 0, $reverse = FALSE, $link = '', $link_remove = '', $subqueue_title = '', $owner = 'nodequeue', $show_in_ui = 1, $show_in_tab = 1, $show_in_links = 1, $reference = 0, $i18n = 1) {
  
  // insert the master noequeue record
  db_query("INSERT INTO {nodequeue_queue}
     (title, subqueue_title, size, link, link_remove, owner, show_in_ui, show_in_tab, show_in_links, reference, reverse, i18n)
     VALUES
     ('%s', '%s', %d, '%s', '%s', '%s', %d, %d, %d, %d, %d, %d)",
     $title, $subqueue_title, $size, $link, $link_remove, $owner, $show_in_ui, $show_in_tab, $show_in_links, $reference, $reverse, $i18n);
  $qid = db_last_insert_id('nodequeue_queue', 'qid');

  if (function_exists('views_invalidate_cache')) {
    views_invalidate_cache();
  }

  // add each role to the nodequeue
  foreach ($roles as $rid) {
    install_nodequeue_add_role($qid, $rid);
  }

  // add each content type to the nodequeue
  foreach ($types as $type) {
    install_nodequeue_add_type($qid, $type);
  }

  // add the subqueue, just pass in the $ref=$qid for now
  install_nodequeue_add_sub($qid, $qid, $title);

  // all done
  return $qid;
}

/**
 * Add a role to a nodequeue.
 *
 * @param $qid
 *   The nodequeue's qid.
 * @param $rid
 *   The user role's rid.
 */
function install_nodequeue_add_role($qid, $rid) {
  db_query("INSERT INTO {nodequeue_roles}
     (qid, rid)
     VALUES
     (%d, %d)",
     $qid, $rid);
}

/**
 * Add a role to a nodequeue.
 *
 * @param $qid
 *   The nodequeue's qid.
 * @param $type
 *   The content type to add.
 */
function install_nodequeue_add_type($qid, $type) {
  db_query("INSERT INTO {nodequeue_types}
     (qid, type)
     VALUES
     (%d, '%s')",
     $qid, $type);
}

/**
 * Add a subqueue to a nodequeue.
 *
 * @param $qid
 *   The nodequeue's qid.
 * @param $ref
 *   The ?
 * @param $title
 *   The 
 * @return
 *   The corresponding sqid (int).
 */
function install_nodequeue_add_sub($qid, $ref, $title) {
  db_query("INSERT INTO {nodequeue_subqueue}
     (qid, reference, title)
     VALUES
     (%d, %d, '%s')",
     $qid, $ref, $title);
  $sqid = db_last_insert_id('nodequeue_subqueue', 'sqid');
  return $sqid;
}
