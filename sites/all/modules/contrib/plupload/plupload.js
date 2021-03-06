// $Id: $

Drupal.behaviors.pluploadBuild = function (context) {
  // Setup uploader pulling some info out Drupal.settings
  $("#uploader").pluploadQueue({
    // General settings
    runtimes : 'html5,flash,html4',
    url : Drupal.settings.plupload.url,
    max_file_size : '10mb',
    chunk_size : '1mb',
    unique_names : false,
    flash_swf_url : Drupal.settings.plupload.swfurl,

    // Specify what files to browse for
    filters : [
      {title : "Image files", extensions : Drupal.settings.plupload.extensions}
    ]
   });
};

Drupal.behaviors.pluploadSuccess = function (context) {
  var totalUploadFiles = 0;
  var upload = $('#uploader').pluploadQueue();               
  upload.bind('FileUploaded', function(up, file, res) {
    totalUploadFiles--;
    if(totalUploadFiles == 0) {
      
      var count = $('#uploader').pluploadQueue().total.uploaded;
      
      var successText = Drupal.formatPlural(count, 'Success! 1 image uploaded.', 'Success! @count images uploaded.');
      
      
      $('div.plupload_header').slideUp('slow', function() {
        $('div.plupload_header').html('<h3>'+ successText +'</h3>').slideDown('slow');
      });
    }
  });

  upload.bind('QueueChanged', function(up, files) {
    totalUploadFiles = upload.files.length;
  });

};
