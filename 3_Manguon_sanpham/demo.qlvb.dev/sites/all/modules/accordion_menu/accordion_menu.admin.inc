<?php
/**
 * @file
 * Provides administrative page callback implementations.
 *
 * @author Jim Berry ("solotandem", http://drupal.org/user/240748)
 */

/**
 * Form builder for the settings form.
 *
 * @see http://jqueryui.com/demos/accordion/ (for jQuery 1.6 options)
 */
function accordion_menu_admin_settings() {
  $form['accordion_menu_number'] = array(
    '#type' => 'textfield',
    '#title' => t('Number of menu blocks'),
    '#description' => t('Enter the number of accordion menu blocks to create (0-99). The accordion menu theme functions may be called by another module or theme even if no blocks are created.'),
    '#default_value' => variable_get('accordion_menus_number', '2'),
    '#size' => 2,
  );
  $form['accordion_menu_scope'] = array(
    '#type' => 'textfield',
    '#title' => t('Scope'),
    '#description' => t('The location in which to place the script. The location must be implemented by the theme. Values include "header" and "footer" by default.'),
    '#default_value' => variable_get('accordion_menu_scope', 'footer'),
    '#size' => 10,
  );

  $form['jquery_noticeable'] = array(
    '#type' => 'fieldset',
    '#title' => t('Noticeable options'),
    '#description' => t('These jQuery Accordion options noticeably affect a menu displayed as an accordion. Some are marked as "undesirable" for use with a menu.'),
    '#tree' => FALSE,
  );
  $form2['accordion_menu_animated'] = array(
    '#type' => 'textfield',
    '#title' => t('Animation'),
    '#description' => t('Enter the name of an animation, or set to "false" to disable them. In addition to the default of "slide", "bounceslide" and all defined easing methods are supported ("bounceslide" requires UI Effects Core). With menu items, the alternative effects are mostly unnoticeable.'),
    '#default_value' => variable_get('accordion_menu_animated', 'slide'),
    '#size' => 10,
  );
  $form2['accordion_menu_auto_height'] = array(
    '#type' => 'checkbox',
    '#title' => t('Auto height'),
    '#description' => t('If checked, the highest content part is used as height reference for all other parts. Provides more consistent animations. <strong>Undesirable</strong>'),
    '#default_value' => variable_get('accordion_menu_auto_height', FALSE),
    '#size' => 2,
  );
  $form2['accordion_menu_clear_style'] = array(
    '#type' => 'checkbox',
    '#title' => t('Clear style'),
    '#description' => t('If checked, clears height and overflow styles after finishing animations. This enables accordions to work with dynamic content, but does not work with auto_height.'),
    '#default_value' => variable_get('accordion_menu_clear_style', FALSE),
    '#size' => 2,
  );
  $form2['accordion_menu_fill_space'] = array(
    '#type' => 'checkbox',
    '#title' => t('Fill space'),
    '#description' => t('If checked, the accordion completely fills the height of the parent element. Overrides auto_height. <strong>Undesirable</strong>'),
    '#default_value' => variable_get('accordion_menu_fill_space', FALSE),
  );
  $form2['accordion_menu_event'] = array(
    '#type' => 'select',
    '#title' => t('Event'),
    '#description' => t('The event on which to trigger the accordion. Suggested use: If the top-level menu item does not link to content, then select "click" to expand the menu. If the top-level menu item links to content, then select "mousedown" to expand the menu and display the linked content. Use "mouseover" at your own risk.'),
    '#default_value' => variable_get('accordion_menu_event', 'mousedown'),
    '#options' => drupal_map_assoc(array('click', 'mousedown', 'mouseover')),
  );
  $form2['accordion_menu_header'] = array(
    '#type' => 'textfield',
    '#title' => t('Header'),
    '#description' => t('Selector for the header element.'),
    '#default_value' => variable_get('accordion_menu_header', 'h3'),
    '#size' => 15,
  );
  $form['jquery_noticeable'] += $form2;

  $form['jquery_unnoticeable'] = array(
    '#type' => 'fieldset',
    '#title' => t('Unnoticeable options'),
    '#description' => t('These jQuery Accordion options do not noticeably affect a menu displayed as an accordion.'),
    '#tree' => FALSE,
  );
  $form3['accordion_menu_collapsible'] = array(
    '#type' => 'checkbox',
    '#title' => t('Collapsible'),
    '#description' => t('Whether all the sections can be closed at once. Allows collapsing the active section by the triggering event.'),
    '#default_value' => variable_get('accordion_menu_collapsible', TRUE),
    '#size' => 2,
  );
  $form3['accordion_menu_navigation'] = array(
    '#type' => 'checkbox',
    '#title' => t('Navigation'),
    '#description' => t('If checked, looks for the anchor that matches location.href and activates it. Great for href-based state-saving. Use navigationFilter to implement your own matcher.'),
    '#default_value' => variable_get('accordion_menu_navigation', FALSE),
  );
  $form['jquery_unnoticeable'] += $form3;

  // Custom validation to make sure the user is entering numbers.
  $form['#validate'][] = 'accordion_menus_settings_validate';

  return system_settings_form($form);
}

/**
 * Validation handler for the settings form.
 *
 * @todo Validate the text options.
 */
function accordion_menus_settings_validate($form, &$form_state) {
  $number = $form_state['values']['accordion_menu_number'];
  // Check to make sure it is a number and that is a maximum of 2 digits.
  if (!is_numeric($number) || strlen($number) > 2) {
    form_set_error('accordion_menu_number', t('You must enter a number from 0 to 99.'));
  }
}
