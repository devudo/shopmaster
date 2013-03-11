<?php
/**
 * @file
 * Helper functions to create actions.
 */

/**
 * Function to create a new email action.
 *
 * @param $description
 *   An optional description for this action.
 * @param $recipient
 *   The email address the message will be sent to.
 * @param $subject
 *   The subject line for this message.
 * @param $body
 *   The body text of this message.
 *
 * @return
 *   The action ID.
 */
function install_add_action_email($description, $recipient, $subject, $body) {
  $parameters = array(
    'recipient' => $recipient,
    'subject' => $subject,
    'message' => $body,
  );
  return install_add_action('system', 'token_actions_send_email_action', $parameters, $description);
}

/**
 * Master function for creating new actions.
 *
 * @param $type
 *   The type of callback used.
 * @param $callback
 *   The name of the callback function.
 * @param $params
 *   An array of parameters to be passed to the callback.
 * @param $description
 *   An optional description for this action.
 *
 * @return
 *   The action ID.
 */
function install_add_action($type, $callback, $params, $description = '') {
  return actions_save($callback, $type, $params, $description);
}
