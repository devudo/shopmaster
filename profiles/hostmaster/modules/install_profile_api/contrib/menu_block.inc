<?php

/**
 * Adds a new menu block.
 *
 * @param $config
 *   An array of parameters to create the menu block. Possible keys are:
 *   - 'title_link' - Make the default block title a link to that menu item.
 *   - 'admin_title' - This title will be used administratively to identify this
 *                     block. If blank, the regular title will be used.
 *   - 'level' - Integer from 1 to 9. Blocks that start with the 1st level will
 *               always be visible. Blocks that start with the 2nd level or
 *               deeper will only be visible when the trail to the active menu
 *               item is in the block’s tree..
 *   - 'follow' - Boolean, If the active menu item is deeper than the level
 *                specified above, the starting level will follow the active
 *                menu item. Otherwise, the starting level of the tree will remain fixed..
 *   - 'depth' - Integer from 0 to 9. From the starting level, specify the
 *               maximum depth of the menu tree, 0 for unlimited.
 *   - 'expanded' - Boolean,  Expand all children of this tree.
 *   - 'sort' - Boolean, Sort each item in the active trail to the top of its
 *              level. When used on a deep or wide menu tree, the active menu
 *              item’s children will be easier to see when the page is reloaded.
 *   - 'menu_name' - The tree of links will only contain children of the selected
 *                   parent item.
 *   - 'parent_mlid' - The tree of links will only contain children of the
 *                     selected parent item.
 */
function install_menu_block_add_block($config = array()) {
  // The adding a menu block 'API' is in the admin file:
  module_load_include('inc', 'menu_block', 'menu_block.admin');

  // Add in the defaults from menu block:
  $values = $config + menu_block_get_config();

  // Determine the delta of the new block.
  $block_ids = variable_get('menu_block_ids', array());
  $delta = empty($block_ids) ? 1 : max($block_ids) + 1;

  // Save the new array of blocks IDs.
  $block_ids[] = $delta;
  variable_set('menu_block_ids', $block_ids);

  // Save the block configuration.
  _menu_block_block_save($delta, $values);

  // Return the newly created menu block delta:
  return $delta;
}
