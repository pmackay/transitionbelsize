<?php

/**
 * @file
 * Group contact form page callbacks for the OG contact module.
 */

/**
 * Group contact page
 */
function og_contact_page() {
  if (!flood_is_allowed('og-contact', variable_get('og_contact_hourly_threshold', 3))) {
    $output = t("You cannot send more than %number messages per hour. Please try again later.", array('%number' => variable_get('og_contact_hourly_threshold', 3)));
  }
  else {
    $output = drupal_get_form('og_contact_mail_page');
  }

  return $output;
}

/**
 * Build the contact form
 */
function og_contact_mail_page(&$form_state) {
  global $user;
  $gid = arg(1);
  $group = og_contact_group_load($gid);

  // use a different theme if the group has one
  $theme_node = og_set_theme($gid);
  og_set_theme($theme_node);
  og_set_group_context($theme_node);

  $name = filter_xss($group->name);
  $admin = og_is_group_admin(node_load($gid));
  if ($admin == TRUE && variable_get('og_contact_group_admin_single', 1) || user_access('administer og contact form') && variable_get('og_contact_group_admin_single', 1)) {
    $form['admin_edit'] = array('#value' => l('edit contact form settings', 'node/'. $gid .'/contact/edit') .'<br /><br />');
  }
  drupal_set_title(t('Contact %name', array('%name' => $name)));
  $groups[$gid] = $name;

  if ($gid && og_contact_group_has_form($gid) == TRUE && arg(2) == 'contact') {
    $form['#token'] = $user->uid ? $user->name . $user->mail : '';
    if (variable_get('og_contact_custom_info', 0) == 1) {
      $form['og_contact_information'] = array('#value' => ($group->info != '') ? filter_xss($group->info) : filter_xss_admin(variable_get('og_contact_form_information', t('You can leave a message using the contact form below.'))));
    }
    else {
      $form['og_contact_information'] = array('#value' => filter_xss_admin(variable_get('og_contact_form_information', t('You can leave a message using the contact form below.'))));
    }
    $form['name'] = array('#type' => 'textfield',
      '#title' => t('Your name'),
      '#maxlength' => 255,
      '#default_value' => $user->uid ? $user->name : '',
      '#required' => TRUE,
    );
    $form['mail'] = array('#type' => 'textfield',
      '#title' => t('Your e-mail address'),
      '#maxlength' => 255,
      '#default_value' => $user->uid ? $user->mail : '',
      '#required' => TRUE,
    );
    $form['subject'] = array('#type' => 'textfield',
      '#title' => t('Subject'),
      '#maxlength' => 255,
      '#required' => TRUE,
    );
    $form['message'] = array('#type' => 'textarea',
      '#title' => t('Message'),
      '#required' => TRUE,
    );
    // We do not allow anonymous users to send themselves a copy
    // because it can be abused to spam people.
    if ($user->uid) {
      $form['copy'] = array('#type' => 'checkbox',
        '#title' => t('Send yourself a copy.'),
      );
    }
    $form['gid'] = array('#type' => 'hidden',
      '#value' => $gid,
    );
    $form['submit'] = array('#type' => 'submit',
      '#value' => t('Send e-mail'),
    );
  }
  else {
    drupal_set_message('Contact form not configured', 'status');
  }
  return $form;
}

/**
 * Validate the OG contact page form submission.
 */
function og_contact_mail_page_validate($form, &$form_state) {
  if (!valid_email_address($form_state['values']['mail'])) {
    form_set_error('mail', t('You must enter a valid e-mail address.'));
  }
}

/**
 * Process the contact page form submission.
 */
function og_contact_mail_page_submit($form, &$form_state) {
  global $language;

  $values = $form_state['values'];
  // E-mail address of the sender: as the form field is a text field,
  // all instances of \r and \n have been automatically stripped from it.
  $from = $values['mail'];

  // Load recipients
  $recipients = og_contact_get_recipients($values['gid']);
  // Load group information
  $group = og_contact_group_load($values['gid']);

  // drupal_mail('og-contact-page-mail', $recipients, $subject,
  drupal_mail('og_contact', 'page_mail', $recipients, language_default(), $values, $from);

  // If the user requests it, send a copy.
  if ($values['copy']) {
  //    drupal_mail('og-contact-page-copy', $from, $subject, $body, $from);
    drupal_mail('og_contact', 'page_copy', $from, $language, $values, $from);
  }

  // Send an auto-reply if necessary:
  if ($group->reply) {
    //   drupal_mail('og-contact-page-autoreply', $from, $subject, wordwrap(check_plain($contact->reply)), $contact->recipients);
    drupal_mail('og_contact', 'page_autoreply', $from, $language, $values);
  }

  // Log the operation:
  flood_register_event('og_contact');
  watchdog('mail', '%name-from sent an e-mail regarding %category.', array('%name-from' => $values['name'] ." [$from]", '%category' => $group->name));

  // Update user:
  drupal_set_message(t('Your message has been sent.'));

  // Jump to group page rather than back to contact page to avoid contradictory messages if flood control has been activated.
  $form_state['redirect'] = ('node/'. $group->gid);
}
