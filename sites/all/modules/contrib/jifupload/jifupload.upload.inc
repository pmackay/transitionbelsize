<?php

/**
 * @file
 * The jifupload uploader sends images to this file, which is a menu callback.
 *
 * Images are validated and saved, new nodes are pre-populated with default values.
 * Tokens are replaced if using token or filefield_paths.
 */

function jifupload_upload($type_name, $field_name, $nid) {
  if (!$_FILES || !$_POST) {
    return;
  }

  // Load cck include and get field instance.
  $field_instance = content_fields($field_name, $type_name);

  // We load the node we're editing and update any changed fields
  // on the form. If there's no $nid we build the new node object.
  $node = node_load($nid);
  if ($nid == 0) {
    $node = new stdClass();
    $node->type = $type_name;
    unset($node->nid);
    include(drupal_get_path('module', 'node'). '/node.pages.inc');
    node_object_prepare($node);
  }

  // Apply any changes to key fields if they are set in $_POST.
  $node_vars = array('title', 'body', 'format', 'comment', 'sticky', 'status', 'promote');
  foreach ($node_vars as $post_key) {
    if (isset($_POST[$post_key])) {
      $node->{$post_key} = $_POST[$post_key];
    }
  }

  // For new nodes where the user has not entered a title, we set one, because
  // it's always good to have a title.
  if (!$node->title) {
    form_set_error('title', $message = 'Please enter a title.', $reset = FALSE);
    $node->title = t('Temporary title - please replace.');
  }

  $filecount = count($_FILES) - 1;
  $dest = file_directory_temp();

  for ($i = 0; $i <= $filecount; $i++) {
    $sourcefilefield = "File" . $i;
    $filename = $_FILES[$sourcefilefield]['tmp_name'];
    $handle = fopen($filename, "r");
    $data = fread($handle, filesize($filename));
    fclose($handle);

    $filepath = $dest .'/'. $_FILES[$sourcefilefield]['name'];

    if ($filepath = file_save_data($data, $filepath)) {

      // If we are using filefield_paths, we replace the token and create the
      // directory if necessary.
      if (module_exists('filefield_paths')) {

        // We reuse some code from filefield paths module.
        $result = db_fetch_object(
          db_query("SELECT filename, filepath FROM {filefield_paths} WHERE type = '%s' AND field = '%s'", $node->type, $field_name)
        );
        $settings['filepath'] = unserialize($result->filepath);
        $destination_path = $settings['filepath']['value'];

        // Replace tokens with values.
        $destination_path = token_replace_multiple($destination_path, $types = array('node' => $node), $leading = '[', $trailing = ']', $options = array());
        jifupload_file_check_directories($destination_path, $mode = 1, $form_item = NULL);

        // Prepend files directory before we save the file.
        $destination_path = file_directory_path() .'/'. $destination_path;
      }

      // If not using filfield_paths check for token module and replace tokens.
      elseif (module_exists('token')) {
        if ($destination_path = filefield_widget_file_path($field_instance)) {
          $destination_path = str_ireplace(file_directory_path(), '', $destination_path);
          if (jifupload_file_check_directories($destination_path, $mode = 1, $form_item = NULL)) {
          }
        }
        // Prepend the files directory before we save the file.
        $destination_path = file_directory_path() .'/'. $destination_path;
      }
      else {
         // Set to save to default file dir, or field path.
        $destination_path = filefield_widget_file_path($field_instance) ? filefield_widget_file_path($field_instance) : file_directory_path();
       // Field path is set, make sure directory exists or create it.
       if (filefield_widget_file_path($field_instance)) {
         $destination_path = str_ireplace(file_directory_path(), '', $destination_path);
         if (jifupload_file_check_directories($destination_path, $mode = 1, $form_item = NULL)) {
         }
         // Prepend the files directory before we save the file.
         $destination_path = file_directory_path() . $destination_path;
        }
      }
// Get the validators for this field
      $validators = filefield_widget_upload_validators($field_instance);

      // Save the new file
      if (!$file = field_file_save_file($filepath, $validators, $destination_path)) {
        watchdog('JIF Upload', t('Could not save file'), NULL, WATCHDOG_ERROR);
        file_delete($filepath);
        continue;
      }

      // Watchdog for successful file upload.
      watchdog('jifupload', t('%Image uploaded.', array('%Image' => $file['filename'])));
      file_delete($filepath);

      // Add the file info to the node.
      $node->{$field_name}[] = $file;

      // Build an array of new fids to use later.
      $newfids[] = $file['fid'];
    }

    // Something is wrong with the file. Message to user.
    else {
      drupal_set_message($message = 'The file '. $file->filename .' could not be saved.', $type = 'status', $repeat = TRUE);
      file_delete($filepath);
    }
  }

  // Save the node. Don't redo pathauto alias.
  if (module_exists('pathauto')) {
    if ($nid == 0) {
      $node->pathauto_perform_alias = TRUE;
    }
    else {$node->pathauto_perform_alias = FALSE;}
  }
  node_save($node);

  // Check for default alt and title, replace tokens if module exists then add
  // to the field's content table.
  if ($field_instance['widget']['alt']) {
    $altdata['alt'] =  $field_instance['widget']['alt'];
  }
  if ($field_instance['widget']['title']) {
    $altdata['title'] =  $field_instance['widget']['title'];
  }

  if (isset($altdata)) {
    if (module_exists('token')) {
      $altdata = token_replace($altdata, $types = 'global', $object = NULL, $leading = '[', $trailing = ']', $options = array());
    }

    $altdata = serialize($altdata);
    $jiftable = 'content_'. $field_name;
    $jifcolumn = $field_name .'_data';
    $jiffid = $field_name .'_fid';

    foreach ($newfids as $key => $fid) {
      $query = "UPDATE {%s} SET %s = '%s' WHERE %s = %d";
      db_query($query, $jiftable, $jifcolumn, $altdata, $jiffid, $fid);
    }
  }

  // Clear the cache so that the new records are loaded into the edit form.
  cache_clear_all('content:'. $node->nid .':'. $node->vid, 'cache_content');
  cache_clear_all('form_'. $_POST['form_build_id'], 'cache_form');

  unset($_FILES);
  unset($_POST);
  echo "SUCCESS";
  exit;
}
