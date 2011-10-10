<?php

/**
 * @file
 * Admin page callbacks for the OG contact module.
 */

/**
 * group/list tab.
 */
function og_contact_admin_groups() {
  $result = db_query('SELECT ogc.gid,n.title FROM {og_contact} ogc INNER JOIN {node} n ON ogc.gid=n.nid WHERE n.status=1');
  $rows = array();
  while ($group = db_fetch_object($result)) {
    $rows[] = array($group->title, l(t('edit'), 'admin/og/contact/edit/'. $group->gid), l(t('delete'), 'admin/og/contact/delete/'. $group->gid));
  }
  $header = array(t('Group'), array('data' => t('Operations'), 'colspan' => 2));

  return theme('table', $header, $rows);
}

/**
 * Build the basic form for adding and editing group contact forms
 */
function og_contact_admin_group_form_skel($op, $gid = NULL) {
  switch ($op) {
    case 'add':
      $options = og_contact_group_select_form_options();
      if (count($options) == 0) {
        $form['group'] = array(
          '#value' => t('All groups already have forms enabled.'),
          );
          return $form;
      }
      $form['group'] = array('#type' => 'select',
        '#title' => t('Group'),
        '#description' => t("Choose the group that you want to add a contact form to."),
        '#required' => TRUE,
        '#options' => $options
      );
      break;
    case 'edit':
      $edit = db_fetch_array(db_query('SELECT * FROM {og_contact} WHERE gid=%d', $gid));
      $form['group'] = array('#type' => 'hidden',
        '#value' => $gid,
      );
      break;
  }
  $form['not_public'] = array(
    '#type' => 'checkbox',
    '#title' => t('Only allow group members to access contact form.'),
    '#default_value' => (!$edit) ? variable_get('not_public', 0) : $edit['notpublic'],
    '#description' => t('This allows you to restrict contact form submissions only to members of this group.'),
  );

  // Recipients section
  $form['recipients'] = array('#type' => 'fieldset',
    '#title' => t('Recipients'),
    '#description' => t('By default, contact form submissions go to the administrators of a group. You can change this default behavior, by adding additional recipients below.'),
  );
  $form['recipients']['no_admin'] = array(
    '#type' => 'checkbox',
    '#title' => t('Do not send form submissions to group administrators.'),
    '#default_value' => (!$edit) ? variable_get('no_admin', 0) : $edit['noadmin'],
    '#description' => t("This allows you to send contact form submissions only to the recipients list."),
  );
  if (variable_get('og_contact_group_recipients', 0) == 1) {
    $form['recipients']['group_recipients'] = array('#type' => 'checkbox',
      '#title' => t('Send form submissions to eligible group members.'),
      '#default_value' => (!$edit) ? variable_get('og_contact_group_recipients', 0) : $edit['grouprecipients'],
      '#description' => t('Form submissions will go to group members in a role with "receive og contact submissions" permission.'),
    );
  }
  $form['recipients']['recipients'] = array('#type' => 'textarea',
    '#title' => t('Additional Recipients'),
    '#default_value' => (!$edit) ? NULL : $edit['recipients'],
    '#description' => t("Form submissions will go to these addresses") .' <br /><br /> '. t("Example: 'webmaster@example.com' or 'sales@example.com,support@example.com'. To specify multiple recipients, separate each e-mail address with a comma."),
  );

  if (variable_get('og_contact_custom_info', 0) == 1) {
    if (variable_get('og_contact_info_admin_allow', 0) == 0 || user_access('administer og contact form')) {
      $form['info'] = array('#type' => 'textarea',
        '#title' => t('Additional Information'),
        '#default_value' => $edit['info'],
        '#description' => t('Information to show on the contact page. Can be anything from submission guidelines to your postal address or telephone number. Leave blank to use the site\'s default group contact form information.'),
      );
    }
  }
  $form['reply'] = array('#type' => 'textarea',
    '#title' => t('Auto-reply'),
    '#default_value' => (!$edit) ? NULL : $edit['reply'],
    '#description' => t('Optional auto-reply. Leave empty if you do not want to send the user an auto-reply message.'),
  );
  $form['submit'] = array('#type' => 'submit',
    '#value' => t('Submit'),
  );

  return $form;
}

/**
 * Group contact add page.
 */
function og_contact_admin_add($gid = NULL) {
  $form = og_contact_admin_group_form_skel('add');
  return $form;
}

/**
 * Validate the group contact add form submission.
 */
function og_contact_admin_add_validate($form, &$form_state) {
  if ($form_state['values']['no_admin'] == 1 && !$form_state['values']['recipients'] && $form_state['values']['group_recipients'] != 1) {
    form_set_error('recipients', t('You must add recipients, or send form submissions to group administrators.'));
  }
  elseif ($form_state['values']['no_admin'] == 1 && $form_state['values']['group_recipients'] == 1 && !$form_state['values']['recipients']) {
    if (!og_contact_check_group_recipients($form_state['values']['group']['#value'])) {
      form_set_error('group_recipients', t('There are currently no members of this group, who are not already administrators, that can recieve form submissions, please add recipients, or send submissions to group administrators.'));
    }
  }
  elseif ($form_state['values']['recipients']) {
    $recipients = explode(',', $form_state['values']['recipients']);
    foreach ($recipients as $recipient) {
      if (!valid_email_address(trim($recipient))) {
        form_set_error('recipients', t('%recipient is an invalid e-mail address.', array('%recipient' => $recipient)));
      }
    }
  }
}

/**
 * Process the group contact add form submission.
 */
function og_contact_admin_add_submit($form, &$form_state) {
  if (arg(3) == 'add') {
    db_query("INSERT INTO {og_contact} (gid, reply,recipients,noadmin,notpublic,info) VALUES (%d,'%s','%s', %d, %d, '%s')", $form_state['values']['group'], $form_state['values']['reply'], $form_state['values']['recipients'], $form_state['values']['no_admin'], $form_state['values']['not_public'], $form_state['values']['info']);
    drupal_set_message(t('Contact form for %group added.', array('%group' => $name)));
    watchdog('mail', 'OG Contact: Form for %group added.', array('%group' => $name), WATCHDOG_NOTICE, l(t('view'), 'admin/og/contact'));
  }
  $form_state['redirect'] = 'admin/og/contact';
  $form_state['nid'] = $node->nid;
}

/**
 * Group contact edit page.
 */
function og_contact_admin_edit() {
  global $user;
  if (arg(0) == 'admin' && is_numeric(arg(4)) && arg(4) > 0) {
    $gid = arg(4);
    $return = 'admin';
  }
  elseif (arg(0) == 'node' && is_numeric(arg(1)) && arg(1) > 0) {
    $gid = arg(1);
    $group = node_load($gid);
    $return = 'node';
  }
  if ($gid) {
    drupal_set_title('Edit contact form for '. og_contact_get_group_name($gid));
    $form = og_contact_admin_group_form_skel('edit', $gid);
    $form['return'] = array('#type' => 'value',
      '#value' => $return
      );
    return $form;
  }
  else {
    if ($return == 'admin') {
      drupal_set_message('You must choose a group to edit.', 'error');
    }
    elseif ($return == 'node') {
      drupal_set_title('Edit contact form for '. og_contact_get_group_name($gid));
      drupal_set_message('You are not an administrator for this group.', 'error');
    }
  }
}

/**
 * Validate the group contact edit form submission.
 */
function og_contact_admin_edit_validate($form, &$form_state) {
  if ($form_state['values']['no_admin'] == 1 && !$form_state['values']['recipients'] && $form_state['values']['group_recipients'] == 0) {
    form_set_error('recipients', t('You must add recipients, or send form submissions to list administrators.'));
  }
  elseif ($form_state['values']['recipients']) {
    $recipients = explode(',', $form_state['values']['recipients']);
    foreach ($recipients as $recipient) {
      if (!valid_email_address(trim($recipient))) {
        form_set_error('recipients', t('%recipient is an invalid e-mail address.', array('%recipient' => $recipient)));
      }
    }
  }
}

/**
 * Process the group contact edit page form submission.
 */
function og_contact_admin_edit_submit($form, &$form_state) {
  $name = og_contact_get_group_name($form_state['values']['group']);
  drupal_set_title('Edit '. $name .'\'s Contact');
  db_query("UPDATE {og_contact} SET reply = '%s', noadmin = %d, grouprecipients = %d, recipients = '%s', notpublic = %d, info='%s' WHERE gid = %d", $form_state['values']['reply'], $form_state['values']['no_admin'], $form_state['values']['group_recipients'], $form_state['values']['recipients'], $form_state['values']['not_public'], $form_state['values']['info'], $form_state['values']['group']);

  drupal_set_message(t('Contact form for %group has been updated.', array('%group' => $name)));
  watchdog('mail', 'OG Contact: Form for %group updated.', array('%group' => $name), WATCHDOG_NOTICE, l(t('view'), 'admin/og/contact'));
  if ($form_state['values']['return'] == 'admin') {
    $form_state['redirect'] = 'admin/og/contact';
    $form_state['nid'] = $node->nid;
  }
  elseif ($form_state['values']['return'] == 'node') {
    $form_state['redirect'] = 'node/'. $form_state['values']['group'] .'/contact';
    $form_state['nid'] = $node->nid;
  }
}

/**
 * Group contact form delete page.
 */
function og_contact_admin_delete($gid = NULL) {
  $name = og_contact_get_group_name($gid);
  if ($info = db_fetch_object(db_query("SELECT gid FROM {og_contact} WHERE gid = %d", $gid))) {
    $form['group'] = array(
      '#type' => 'value',
      '#value' => $info->gid,
    );

    return confirm_form($form, t('Are you sure you want to delete the contact form for %group?', array('%group' => $name)), 'admin/og/contact', t('Any text in the auto reply field will be lost if this form is deleted.'), t('Delete'), t('Cancel'));
  }
  else {
    drupal_set_message(t('Group not found.'), 'error');
    drupal_goto('admin/og/contact');
  }
}

/**
 * Process group delete form submission.
 */
function og_contact_admin_delete_submit($form, &$form_state) {
  $name = og_contact_get_group_name($form_state['values']['group']);
  db_query("DELETE FROM {og_contact} WHERE gid = %d", arg(4));
  drupal_set_message(t('The group contact form for %group has been deleted.', array('%group' => $name)));
  watchdog('mail', 'OG Contact: Form for %group was deleted.', array('%group' => $name), WATCHDOG_NOTICE);

  $form_state['redirect'] = 'admin/og/contact';
  $form_state['nid'] = $node->nid;
}

/**
 * OG Conact admin settings page
 */
function og_contact_admin_settings() {
  $form['og_contact_hourly_threshold'] = array('#type' => 'select',
    '#title' => t('Hourly threshold'),
    '#options' => drupal_map_assoc(array(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 20, 30, 40, 50)),
    '#default_value' => variable_get('og_contact_hourly_threshold', 3),
    '#description' => t('The maximum number of contact form submissions a user can perform per hour.'),
  );
  $form['og_contact_additonal_info'] = array('#type' => 'fieldset',
    '#title' => t('Additional Information'),
  );
  $form['og_contact_additonal_info']['og_contact_form_information'] = array('#type' => 'textarea',
    '#title' => t('Default Additional information Text'),
    '#default_value' => variable_get('og_contact_form_information', t('You can leave a message using the contact form below.')),
    '#description' => t('Information to show on the contact page. Can be anything from submission guidelines to your postal address or telephone number.'),
  );
  $form['og_contact_additonal_info']['og_contact_custom_info'] = array('#type' => 'checkbox',
    '#title' => t('Custom "Additional Information" Fields'),
    '#description' => t('Enable additional information fields for each contact form.'),
    '#default_value' => variable_get('og_contact_custom_info', 0),
  );
  $form['og_contact_additonal_info']['og_contact_info_admin_allow'] = array('#type' => 'checkbox',
    '#title' => t('Do not allow group admins to write Additional Information text.'),
    '#description' => t('This will prevent group administrators with "administer group contact form" from being able to modify the Additional Information field.'),
    '#default_value' => variable_get('og_contact_info_admin_allow', 0),
  );
  $form['og_contact_global_group'] = array('#type' => 'fieldset',
    '#title' => t('Global group contact form settings.'),
  );
  $form['og_contact_global_group']['og_contact_display_tab'] = array(
    '#type' => 'checkbox',
    '#title' => t('Display "Contact" tabs for groups'),
    '#default_value' => variable_get('og_contact_display_tab', 1),
    '#description' => t('Add a "Contact" tab to the tabs of any group with a contact form enabled. If this is turned off, contact forms can still be reached at the URL "node/nid/contact."'),
  );
  $form['og_contact_global_group']['og_contact_group_name_in_subject'] = array(
    '#type' => 'checkbox',
    '#title' => t('Send contact form emails with group name in the subject.'),
    '#default_value' => variable_get('og_contact_group_name_in_subject', 0),
    '#description' => t('When a contact form is submitted, the name of the group that is receiving the submission will be prepended to the user supplied subject. (eg: "[Group Name] User supplied subject")'),
  );

  // new groups defaults
  $form['og_contact_new_groups'] = array('#type' => 'fieldset',
    '#title' => t('New group defaults'),
  );
  $form['og_contact_new_groups']['intro'] = array(
    '#value' => t('NOTE: These changes only effect new OG Contact forms. They do not take effect retroactively. You must change values for each contact form that has already been created, or delete and recreate all of them.'),
  );
  $form['og_contact_new_groups']['og_contact_enable_all'] = array(
    '#type' => 'checkbox',
    '#title' => t('Automatically create Contact forms for new groups'),
    '#default_value' => variable_get('og_contact_enable_all', 0),
    '#description' => t('Give all new groups Contact forms when they are created. NOTE: existing groups will still need to have their Contact forms enabled, you can enable them for all groups '. l('here', 'admin/og/contact/addall') .'.'),
  );
  $form['og_contact_new_groups']['og_contact_group_not_public'] = array(
    '#type' => 'checkbox',
    '#title' => t('Make contact forms private by default.'),
    '#default_value' => variable_get('og_contact_group_not_public', 0),
    '#description' => t('All newly created groups will have their contact forms set to private by default. This can still be overriden on each form\'s settings page. All existing contact forms will keep their current privacy settings.'),
  );

  $form['og_contact_perm_options'] = array('#type' => 'fieldset',
    '#title' => t('Permission based options'),
  );
  $form['og_contact_perm_options']['og_contact_group_admin_single'] = array(
    '#type' => 'checkbox',
    '#title' => t('Allow users with "administer group contact form" permissions to edit forms'),
    '#default_value' => variable_get('og_contact_group_admin_single', 0),
    '#description' => t('Users that are in a role with the "administer group contact form" permission, and are administrators of a given group, will be able to edit that group\'s Contact Form settings.'),
  );
  $form['og_contact_perm_options']['og_contact_group_recipients'] = array(
    '#type' => 'checkbox',
    '#title' => t('Allow form recipients by role.'),
    '#default_value' => variable_get('og_contact_group_recipients', 0),
    '#description' => t('Users that are in a role with the "receive og contact submissions" permission, and are members of a given group, will receive mail from the contact form.'),
  );
  return system_settings_form($form);
}

/**
 * Add all Groups page.
 */
function og_contact_admin_add_all() {
  $group_count = og_contact_og_count();
  $form_count = og_contact_form_count();
  if ($group_count == $form_count && $group_count != 0) {
    $form['all_groups'] = array(
      '#value' => t('All groups already have forms enabled.'),
      );
    return $form;
  }
  $form['all_groups'] = array('#type' => 'value',
    '#value' => 1,
  );
  return confirm_form($form, t('Are you sure you want to add contact forms for every group?'), 'admin/og/contact', t(''), t('Add'), t('Cancel'));
}

/**
 * Process add all Groups form submission.
 */
function og_contact_admin_add_all_submit($form, &$form_state) {
  $result =  db_query('SELECT nid FROM {og}');
  $grpno = 0;
  $alteredno = 0;
  if (variable_get('og_contact_group_not_public', 0) === 1) {
    $notpublic = 1;
  }
  else {
    $notpublic = 0;
  }
  while ($gid = db_fetch_object($result)) {
    //Check if group doesn't already have a contact form
    if (!og_contact_group_has_form($gid->nid)) {
      //It doesn't, so add it to the og_contact table
      db_query("INSERT INTO {og_contact} (gid, reply, notpublic) VALUES (%d,'%s', %d)", $gid->nid, "", $notpublic);
      $alteredno++;
    }
    $grpno++;
  }
  if ($grpno != $alteredno) {
    drupal_set_message(t('Added Contact forms to the %altered groups (of %groups) that did not already have forms.', array('%altered' => $alteredno, '%groups' => $grpno)));
    watchdog('mail', 'OG Contact: Added contact form to the %altered groups (of %groups) that did not already have forms.', array('%altered' => $alteredno, '%groups' => $grpno), WATCHDOG_NOTICE);
  }
  else {
    drupal_set_message(t('Added Contact forms for all groups'));
    watchdog('mail', 'OG Contact: Added Contact forms for all groups', array(), WATCHDOG_NOTICE);
  }
  $form_state['redirect'] = 'admin/og/contact';
  $form_state['nid'] = $node->nid;
}

/**
 * Delete all Groups page.
 */
function og_contact_admin_delete_all() {
  $form_count = og_contact_form_count();
  if ($form_count == 0) {
    $form['all_groups'] = array(
      '#value' => t('There are currently no contact forms enabled.'),
      );
    return $form;
  }
  $form['all_groups'] = array('#type' => 'value',
    '#value' => 1,
  );
  return confirm_form($form, t('Are you sure you want to delete the contact forms for every group?'), 'admin/og/contact', t('Any group specific settings and information will be lost. This action cannot be undone.'), t('Delete'), t('Cancel'));
}

/**
 * Process delete all Groups form submission.
 */
function og_contact_admin_delete_all_submit($form, &$form_state) {
  db_query('DELETE FROM {og_contact}');
  drupal_set_message(t('The group contact form for all groups have been deleted.'));
  watchdog('mail', 'OG Contact: Forms for all groups were deleted.', array(), WATCHDOG_NOTICE);

  $form_state['redirect'] = 'admin/og/contact';
  $form_state['nid'] = $node->nid;
}

