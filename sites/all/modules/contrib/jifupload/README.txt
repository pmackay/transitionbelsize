
/**
 * @file
 * README.txt file for jUpload for Imagefield module.
 */

Description
The  jUpload for Imagefield module integrates the jupload Java applet with
Drupal. Multiple images are uploaded in a single batch into a multiple-value
imagefield.

Requirements
 - Modules: CCK, filefield, imagefield
 - Users must have Java installed.
 - This module includes JUpload version 4.3.2 [SVN-Rev: 735].
   Note that this is the wjhk jupload. There are more than one java uploaders
   named jupload.

Configurable features
 - Selection of content type used.
 - Selection of imagefield field used.
 - Size of the uploader on the page.
 - jUpload parameters may also be entered manually in the module's admin page.

Benefits
 - Allows batch image uploading to imagefield.
 - Set image descriptions in the imagefield.
 - Deletions are handled by imagefield and filefield.
 - Image types are limited to jpg, gif, png.

Limitations
 - The current dev version does not yet handle other cck fields well.
   If you have other cck fields in the content type, changes may be lost when
   you upload images.

Suggestions
 - With filefield_paths and token modules you can customize the directories
   where the images are saved. Otherwise images are saved in the default files
   directory.
 - With imagecache thumbnails and other sizes can be created automatically.

Installation
 - See INSTALL.txt

Theming
 - The uploader is enclosed in a <div>. The id is [field_fieldname-applet].
 - Theming of images after uploading is up to you.

Additional
 - The to-do list includes ahah or ajax loading of newly uploaded images.

Author
awolfey
aaronewolfe@gmail.com