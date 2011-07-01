<?php
// $Id: template.php,v 1.21 2009/08/12 04:25:15 johnalbin Exp $

/**
 * @file
 * Contains theme override functions and preprocess functions for the theme.
 *
 * ABOUT THE TEMPLATE.PHP FILE
 *
 *   The template.php file is one of the most useful files when creating or
 *   modifying Drupal themes. You can add new regions for block content, modify
 *   or override Drupal's theme functions, intercept or make additional
 *   variables available to your theme, and create custom PHP logic. For more
 *   information, please visit the Theme Developer's Guide on Drupal.org:
 *   http://drupal.org/theme-guide
 *
 * OVERRIDING THEME FUNCTIONS
 *
 *   The Drupal theme system uses special theme functions to generate HTML
 *   output automatically. Often we wish to customize this HTML output. To do
 *   this, we have to override the theme function. You have to first find the
 *   theme function that generates the output, and then "catch" it and modify it
 *   here. The easiest way to do it is to copy the original function in its
 *   entirety and paste it here, changing the prefix from theme_ to tb_.
 *   For example:
 *
 *     original: theme_breadcrumb()
 *     theme override: tb_breadcrumb()
 *
 *   where tb is the name of your sub-theme. For example, the
 *   zen_classic theme would define a zen_classic_breadcrumb() function.
 *
 *   If you would like to override any of the theme functions used in Zen core,
 *   you should first look at how Zen core implements those functions:
 *     theme_breadcrumbs()      in zen/template.php
 *     theme_menu_item_link()   in zen/template.php
 *     theme_menu_local_tasks() in zen/template.php
 *
 *   For more information, please visit the Theme Developer's Guide on
 *   Drupal.org: http://drupal.org/node/173880
 *
 * CREATE OR MODIFY VARIABLES FOR YOUR THEME
 *
 *   Each tpl.php template file has several variables which hold various pieces
 *   of content. You can modify those variables (or add new ones) before they
 *   are used in the template files by using preprocess functions.
 *
 *   This makes THEME_preprocess_HOOK() functions the most powerful functions
 *   available to themers.
 *
 *   It works by having one preprocess function for each template file or its
 *   derivatives (called template suggestions). For example:
 *     THEME_preprocess_page    alters the variables for page.tpl.php
 *     THEME_preprocess_node    alters the variables for node.tpl.php or
 *                              for node-forum.tpl.php
 *     THEME_preprocess_comment alters the variables for comment.tpl.php
 *     THEME_preprocess_block   alters the variables for block.tpl.php
 *
 *   For more information on preprocess functions and template suggestions,
 *   please visit the Theme Developer's Guide on Drupal.org:
 *   http://drupal.org/node/223440
 *   and http://drupal.org/node/190815#template-suggestions
 */


/**
 * Implementation of HOOK_theme().
 */
function tb_theme(&$existing, $type, $theme, $path) {
  $hooks = zen_theme($existing, $type, $theme, $path);
  // Add your theme hooks like this:
  $hooks = array(
    // run as: theme('footer', $primary_links, $global_links);
    'footer' => array(
      // routes to file tb/templates/footer.tpl.php
      'template' => 'footer',

      // these variables must be called in the theme function, and will appear in the template
      'arguments' => array(
        'primary_links' => null,
        'global_links' => null,
      ),
      'path' => path_to_theme().'/templates'
    ),

    'event_map' => array(
      // routes to file tb/templates/event-map.tpl.php
      'template' => 'event-map',

      // these variables must be called in the theme function, and will appear in the template
      'arguments' => array(
        'e' => null
      ),
      'path' => path_to_theme().'/templates'
    )

  );

  return $hooks;
}

/**
 * Override or insert variables into all templates.
 *
 * @param $vars
 *   An array of variables to pass to the theme template.
 * @param $hook
 *   The name of the template being rendered (name of the .tpl.php file.)
 */
/*
function tb_preprocess(&$vars, $hook) {
}
// */

/**
 * Override or insert variables into the page templates.
 *
 * @param $vars
 *   An array of variables to pass to the theme template.
 * @param $hook
 *   The name of the template being rendered ("page" in this case.)
 */
function tb_preprocess_page(&$vars, $hook) {
 	$vars['sep'] = ' &gt; ';
  if(arg(0) == 'date-browser') {
  	$vars['title'] = 'Calendar';
  }
  
  /*
  if(drupal_is_front_page()) {
	  $map = theme('image', path_to_theme().'/images/map.gif');
	  $link = 'http://maps.google.com/maps?oe=UTF-8&gfns=1&q=Belsize+Park,+London,+United+Kingdom&um=1&ie=UTF-8&hq=&hnear=Belsize+Park,+Greater+London,+UK&ei=rkkCTbyvO9DHswaYurTtCQ&sa=X&oi=geocode_result&ct=image&resnum=1&ved=0CBcQ8gEwAA';
	   
	  $map = l($map, $link, array('attributes' => array('target' => '_blank'), 'html' => 'true'));
    $enlarge = l(t('enlarge'), $link, array('attributes' => array('target' => '_blank'), 'html' => 'true'));
    $vars['map'] = $map . '<div id="enlarge-map">'. $enlarge .'</div><div class="clear" id="loc_bottom"></div>';
  }*/

  _add_footer_links($vars);
}

function _add_footer_links(&$vars) {

  $footer_primary = theme(array('links__system_main_menu', 'links'), menu_primary_links(),
    array(
      'id' => 'footer-menu',
      'class' => 'links-foot clearfix', // links changed to links-foot for styling GR 
    ),
    array(
      'text' => t('Main menu'),
      'level' => 'h2',
      'class' => 'element-invisible',
    )
  );


  $global_links = array(
    'transitionnetwork.org',
    'abundance.org',
    'transitionfinsburypark.org.uk',
    'permaculture.org.uk'
  );
  foreach($global_links as $k => $link) {
    $options = array(
      'attributes' => array(
        'target' => '_blank',
        'class' => 'global-link'
      )
    );
    $global_links[$k] = l($link, 'http://'.$link, $options);
  }
  $footer_global = theme('item_list', $global_links, NULL, 'ul', array('id'=>'global-links'));


  $vars['footer'] .= theme('footer', $footer_primary, $footer_global);
}

/**
 * Override or insert variables into the node templates.
 *
 * @param $vars
 *   An array of variables to pass to the theme template.
 * @param $hook
 *   The name of the template being rendered ("node" in this case.)
 */
function tb_preprocess_node(&$vars, $hook) {
		
  // Optionally, run node-type-specific preprocess functions, like
  // tb_preprocess_node_page() or tb_preprocess_node_story().
  $function = __FUNCTION__ . '_' . $vars['node']->type;
  if (function_exists($function)) {
    $function($vars, $hook);
  }
}

function tb_preprocess_node_event(&$vars, $hook) {
  // event date
  $vars['event_date'] = _get_event_date($vars['node']->field_date[0]['value']);

  // location
  $vars['location'] = _get_event_location($vars['node']->field_location[0]['nid']);

  // organiser
  $event_user = user_load($vars['node']->uid);
  $title = 'View user profile';
  $path = 'users/'. $event_user->name;
  $vars['submitted'] = l($event_user->name, $path, array('attributes' => array('title' => $title)));

  // RSVP: Attend event
  $vars['attend_event'] = get_attend_event_button($vars['node']);

  // event google map
  $vars['google_map'] = _get_event_map($vars['node']->field_location[0]['nid']);
}

function tb_preprocess_node_gallery(&$vars, $hook) {
  if($vars['teaser']) {
    $path = $vars['field_images'][0]['filepath'];
    $presetname = 'event_thumb';
    $img = theme('imagecache', $presetname, $path, $alt = '', $title = '');
    $output = l($img, "node/{$vars['nid']}", array('html' => TRUE));
    $vars['content'] = $output;
  }
}
/**
 * Override or insert variables into the comment templates.
 *
 * @param $vars
 *   An array of variables to pass to the theme template.
 * @param $hook
 *   The name of the template being rendered ("comment" in this case.)
 */
/* -- Delete this line if you want to use this function
function tb_preprocess_comment(&$vars, $hook) {
  $vars['sample_variable'] = t('Lorem ipsum.');
}
// */

/**
 * Override or insert variables into the block templates.
 *
 * @param $vars
 *   An array of variables to pass to the theme template.
 * @param $hook
 *   The name of the template being rendered ("block" in this case.)
 */
/* -- Delete this line if you want to use this function
function tb_preprocess_block(&$vars, $hook) {
  $vars['sample_variable'] = t('Lorem ipsum.');
}
// */

/**
 * Format the "Submitted by username on date/time" for each node
 *
 * @ingroup themeable
 */
function tb_node_submitted($node) {
	if($node->type == 'event') {
		$text = 'Author: !username';				
	} else {
		$text = 'Submitted by !username on @datetime';
	}
  return t($text,
    array(
      '!username' => theme('username', $node),
      '@datetime' => format_date($node->created),
    ));
}

/*****
 * overwrite cck filed(s) // NOT IN USE CHECK PREPROCESS EVENT
 */
function tb_preprocess_content_field($vars) {
	if($vars['field']['field_name'] == 'field_location') {
		$location = node_load($vars['items'][0]['nid']);
		
		if($location->field_building_nr[0]['value']) $code['building_nr'] = $location->field_building_nr[0]['value'];
		if($location->field_street[0]['value']) $code['street'] = $location->field_street[0]['value'];
		if($location->field_city[0]['value']) $code['city'] = $location->field_city[0]['value'];
		if($location->field_conutry[0]['value']) $code['country'] = $location->field_conutry[0]['value'];
		if($location->field_postcode[0]['value']) $code['postcode'] = $location->field_postcode[0]['value'];
		
		if($code) {
			$address = implode(', ', $code);			
		} else {
			$address = 'London';
		}
		$width = 370;
	  $height = 220;
	  $src = $map = 'http://maps.google.com/maps?f=q&source=s_q&geocode=&q='. $address .'&ie=UTF8&z=16&output=embed';
	 	$map = '<iframe width="'. $width .'" height="'. $height .'" frameborder="0" scrolling="no" marginheight="0" marginwidth="0" src="'. $src .'"></iframe><p class="embed-gmap-link"><a href="'. str_replace('&amp;output=embed', '', $src) .'" target="_BLANK">'. t('enlarge') .'</a></p>';
			
		$vars['google_map'] = $map;		
	}
}

function tb_preprocess_views_view_table(&$vars) {
	if($vars['view']->vid == 4) {
		// add image to each offer
		foreach($vars['rows'] as $count => $row) {
			$nid = $vars['view']->result[$count]->nid;
			$num_attendees = get_attendees($nid);	
			if(is_array($num_attendees)) {
				$num_attendees = count($num_attendees);
			} else {
				$num_attendees = 0;
			}

      if(empty($row['field_max_attendees_value'])) {
				$vars['rows'][$count]['field_max_attendees_value'] = $num_attendees. '/' .variable_get('rsvp_default_max_guests', 123);
			} else {
        $vars['rows'][$count]['field_max_attendees_value'] = $num_attendees. '/' .$vars['rows'][$count]['field_max_attendees_value'];
      }
		}
	
		// add empty header
		$vars['header']['num_attendees'] = 'Atnd.';
		
	}
	
}

function tb_preprocess_views_view_fields__block_1(&$vars) {
  // get location title
  $view_row_id = $vars['id']-1;
  $event = node_load($vars['view']->result[$view_row_id]->nid);
  $location = node_load($event->field_location[0]['nid']);

  // add field location to view row
  $flocation = new stdClass();
  $flocation->content = $location->title;
  $flocation->inline = FALSE;
  $flocation->inline_html = 'div';
  $flocation->class = 'location';
  $flocation->element_type = 'span';
  $flocation->label = '';
  $vars['fields']['location_name'] = $flocation;
}


function get_attend_event_button($event) {
	if(user_is_logged_in()) {
		
		// ..if max attendees is reached or above then STOP subscribing 
		// and we not attend for this event
		if($event->field_max_attendees[0]['value']) {
			$max = $event->field_max_attendees[0]['value'];
		} else {
			$max = variable_get('rsvp_default_max_guests', 123);
		}
		if(count(get_attendees_uids($event->nid)) >= $max && !user_allready_attend($event->nid)) {
			$button = t('Max attendees for this event are reached.');
		} else {

			if(user_allready_attend($event->nid)) {
				$button = t('Your are allready have attend for this event'). '&nbsp; ('. l('Unsubscribe', "node/{$event->nid}/unsubscribe"). ')';
			} else {
				$button = l(t('Attend Event'), "node/{$event->nid}/attend");
			}			
		}
		
	} else { // for anonymouse users..
		$logregvars = array(
			'!l' => l('Login', 'user', array('query' => drupal_get_destination())), 
			'!r' => l('register', 'user/register', array('query' => drupal_get_destination()))
		);
		$button = t('!l or !r to attend events', $logregvars);
	}
	
	return 'RSVP: '. $button;
}

function user_allready_attend($event_nid) {
	global $user;
	$auids = get_attendees_uids($event_nid);

	if(is_array($auids)) {
		return in_array($user->uid, $auids) ? TRUE : FALSE;
	} else {
		return FALSE;
	}
}

function tb_breadcrumb($breadcrumb) {
  $sep = ' &gt; ';
  if (count($breadcrumb) > 0) {
    return implode($breadcrumb, $sep);
  }
  else {
    return t("Home");
  }
}

function _get_event_location($location_nid) {
  $node = node_load($location_nid);
  $bul = $node->field_building_nr[0]['value'];
  $street = $node->field_street[0]['value'];
  $city = $node->field_city[0]['value'];
  $conutry = $node->field_conutry[0]['value'];
  $postcode = $node->field_postcode[0]['value'];

  $location = '<ul class="address">';
  $location .= '<li>'. $bul.' '. $street. ',</li>';
  $location .= '<li>'. $city. ',</li>';
  $location .= '<li>'. $postcode. '</li>';
  $location .= '</ul>';

  return $location;
}

function _get_event_map($location_nid) {
  $location = node_load($location_nid);

  if($location->field_building_nr[0]['value']) $code['building_nr'] = $location->field_building_nr[0]['value'];
  if($location->field_street[0]['value']) $code['street'] = $location->field_street[0]['value'];
  if($location->field_city[0]['value']) $code['city'] = $location->field_city[0]['value'];
  if($location->field_conutry[0]['value']) $code['country'] = $location->field_conutry[0]['value'];
  if($location->field_postcode[0]['value']) $code['postcode'] = $location->field_postcode[0]['value'];

  if($code) {
    $address = implode(', ', $code);
  } else {
    $address = 'London';
  }

  $e = new stdClass();
  $e->width = 302;
  $e->height = 242;
  $e->src = 'http://maps.google.com/maps?f=q&source=s_q&geocode=&q='. $address .'&ie=UTF8&z=16&output=embed&iwloc=near';
  $e->enlarge_link = t('enlarge');
  
  return theme('event_map', $e);
}

function _get_event_date($event_date) {
  list($date, $time) = explode('T', $event_date);
  list($year, $month, $day) = explode('-', $date);
  list($hours, $minutes, $seconds) = explode(':', $time);
  $timestamp = mktime($hours, $minutes, $seconds, $month, $day, $year);

  return format_date($timestamp, 'custom', 'd/m/Y g:ia', $timezone=0);
}

