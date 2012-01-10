<?php
/**
 * @file
 * The jUpload for Imagefield admin settings form.
 *
 * The form provides control of the module as well as the jupload applet itself.
 */

function jifupload_admin_settings() {

  $form['module'] = array(
    '#type' => 'fieldset',
    '#weight' => $group_weight,
    '#title' => t('Module Settings'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  $form['module']['jifupload_node_types'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Select content types that will use the jUpload'),
    '#options' => node_get_types('names'),
    '#default_value' => variable_get('jifupload_node_types', array('')),
    '#description' => t('The uploader will only be available on these content types.'),
  );

  $form['module']['jifupload_field_types'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Select imagefield fields that will use jUpload'),
    '#options' => jifupload_fieldlist(),
    '#default_value' => variable_get('jifupload_field_types', array('')),
    '#description' => t('The uploader will only be available for multiple imagefields with these field names.
                         You must also add the field to your content type. If no fields are listed you will
                         need to create one before you can use the uploader.'),
  );

  $form['applet'] = array(
    '#type' => 'fieldset',
    '#weight' => $group_weight,
    '#title' => t('Applet Settings'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  $form['applet']['jifupload_applet_width'] = array(
    '#type' => 'textfield',
    '#size' => '4',
    '#title' => t('Applet width'),
    '#default_value' => variable_get('jifupload_applet_width', '650'),
    '#description' => t("The width of the embedded applet."),
    '#required' => FALSE,
  );

  $form['applet']['jifupload_applet_height'] = array(
    '#type' => 'textfield',
    '#size' => '4',
    '#title' => t('Applet height'),
    '#default_value' => variable_get('jifupload_applet_height', '400'),
    '#description' => t("The height of the embedded applet."),
    '#required' => FALSE,
  );

  $form['applet']['jifupload_maxfilesize'] = array(
    '#type' => 'textfield',
    '#size' => '3',
    '#title' => t('Maximum individual file size in megabytes'),
    '#default_value' => variable_get('jifupload_maxfilesize', -1),
    '#description' => t("For no limit, set to -1."),
    '#required' => FALSE,
  );

  $form['applet']['jifupload_params'] = array(
    '#type' => 'textarea',
    '#title' => t('Enter additional jUpload parameters here.'),
    '#default_value' => variable_get('jifupload_params', ''),
    '#description' => t("These parameters will appear at the bottom of the parameter list. Therefor, they override
                         the defaults and anything configured above. See http://jupload.sourceforge.net/ for parameter
                         definitions. Note that this module uses JUpload version 3.2.0. Use this format with no spaces:
                         paramname=value. Put each parameter on a separate line.
                        For example:
<pre>
                        showStatusBar=False
                        urlToSendErrorTo=/path/to/upload/error/page
</pre>
                        CAUTION: Incorrect parameter style may cause your uploader or content type to no longer display."),
    '#required' => FALSE,
  );

  return system_settings_form($form);
}

/* Generate a list of suitable fields
 */
function jifupload_fieldlist() {
  $fields = (content_fields());
  foreach ($fields as $field) {
    if (($field['module'] == 'imagefield' || $field['module'] == 'filefield') && $field['active'] && $field['multiple']) {
      $fieldlist[$field['field_name']] = $field['field_name'];
    }
  }
  if (empty($fieldlist)) {
    drupal_set_message(t('There are no multiple-value imagefields. The uploader is only avaiable for multiple-value imagefields.'));
    return;
  }
  return $fieldlist;
}