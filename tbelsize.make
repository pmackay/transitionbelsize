; $Id$
;
; Example makefile
; ----------------
; This is an example makefile to introduce new users of drush_make to the
; syntax and options available to drush_make. For a full description of all
; options available, see README.txt.

; This make file is a working makefile - try it! Any line starting with a `;`
; is a comment.

; Core version
; ------------
; Each makefile should begin by declaring the core version of Drupal that all
; projects should be compatible with.

core = 6.x

; API version
; ------------
; Every makefile needs to declare it's Drush Make API version. This version of
; drush make uses API version `2`.

api = 2

; Core project
; ------------
; In order for your makefile to generate a full Drupal site, you must include
; a core project. This is usually Drupal core, but you can also specify
; alternative core projects like Pressflow. Note that makefiles included with
; install profiles *should not* include a core project.

; Use pressflow instead of Drupal core:
; projects[pressflow][type] = "core"
; projects[pressflow][download][type] = "file"
; projects[pressflow][download][url] = "http://launchpad.net/pressflow/6.x/6.15.73/+download/pressflow-6.15.73.tar.gz"

; CVS checkout of Drupal 6.x core:
; projects[drupal][type] = "core"
; projects[drupal][download][type] = "cvs"
; projects[drupal][download][root] = ":pserver:anonymous:anonymous@cvs.drupal.org:/cvs/drupal"
; projects[drupal][download][revision] = "DRUPAL-6"
; projects[drupal][download][module] = "drupal"

; CVS checkout of Drupal 7.x. Requires the `core` property to be set to 7.x.
; projects[drupal][type] = "core"
; projects[drupal][download][type] = "cvs"
; projects[drupal][download][root] = ":pserver:anonymous:anonymous@cvs.drupal.org:/cvs/drupal"
; projects[drupal][download][revision] = "HEAD"
; projects[drupal][download][module] = "drupal"

;projects[] = drupal
projects[drupal][type] = core

; Projects
; --------
; Each project that you would like to include in the makefile should be
; declared under the `projects` key. The simplest declaration of a project
; looks like this:

; To include the most recent views module:

projects[admin][subdir] = "contrib"
projects[admin_menu][subdir] = "contrib"
projects[backup_migrate][subdir] = "contrib"
projects[calendar][subdir] = "contrib"
projects[cck][subdir] = "contrib"
;projects[ckeditor][subdir] = "contrib"
projects[css_gzip][subdir] = "contrib"
projects[date][subdir] = "contrib"
projects[demo][subdir] = "contrib"
projects[devel][subdir] = "contrib"
projects[embed_gmap][subdir] = "contrib"
projects[filefield][subdir] = "contrib"
projects[gmap][subdir] = "contrib"
projects[google_analytics][subdir] = "contrib"
projects[imageapi][subdir] = "contrib"
projects[imagecache][subdir] = "contrib"
projects[imagefield][subdir] = "contrib"
projects[javascript_aggregator][subdir] = "contrib"
projects[jquery_ui][subdir] = "contrib"
projects[jquery_update][subdir] = "contrib"
projects[lightbox2][subdir] = "contrib"
projects[location][subdir] = "contrib"
projects[logintoboggan][subdir] = "contrib"
projects[module_filter][subdir] = "contrib"
projects[og][subdir] = "contrib"
projects[pathauto][subdir] = "contrib"
projects[poormanscron][subdir] = "contrib"
projects[rsvp][subdir] = "contrib"
projects[simplenews][subdir] = "contrib"
projects[token][subdir] = "contrib"
projects[vertical_tabs][subdir] = "contrib"
projects[views][subdir] = "contrib"
projects[views][type] = "module"

; Libraries
libraries[jquery_ui][download][type] = "get"
libraries[jquery_ui][download][url] = "http://jquery-ui.googlecode.com/files/jquery.ui-1.6.zip"
http://jqueryui.com/download/jquery-ui-1.8.11.custom.zip
libraries[jquery_ui][directory_name] = "jquery.ui"
libraries[jquery_ui][destination] = "modules/contrib/jquery_ui"

;CKEditor
libraries[tinymce][download][type] = "get"
libraries[tinymce][download][url] = "http://download.cksource.com/CKEditor/CKEditor/CKEditor%203.5.3/ckeditor_3.5.3.zip"
libraries[tinymce][directory_name] = "ckeditor"



; This will, by default, retrieve the latest recommended version of the project
; using its update XML feed on Drupal.org. If any of those defaults are not
; desirable for a project, you will want to use the keyed syntax combined with
; some options.

; If you want to retrieve a specific version of a project:

;projects[cck] = 2.6

; Or an alternative, extended syntax:

;projects[ctools][version] = 1.3

; Check out the latest version of a project from CVS. Note that when using a
; repository as your project source, you must explictly declare the project
; type so that drush_make knows where to put your project.

;projects[data][type] = module
;projects[data][download][type] = cvs
;projects[data][download][module] = contributions/modules/data
;projects[data][download][revision] = DRUPAL-6--1

; Clone a project from github.

;projects[tao][type] = theme
;projects[tao][download][type] = git
;projects[tao][download][url] = git://github.com/developmentseed/tao.git

; If you want to install a module into a sub-directory, you can use the
; `subdir` attribute.

;projects[admin_menu][subdir] = custom

; To apply a patch to a project, use the `patch` attribute and pass in the URL
; of the patch.

;projects[admin_menu][patch][] = "http://drupal.org/files/issues/admin_menu.long_.31.patch"
