<?php

/**
 * Implementation of hook_form()
 */
function megamenu_admin_form(&$node, $form_state = NULL) {
  $form = array();

  $form['help'] = array(
    '#type' => 'markup',
    '#value' => t("To enable a Mega menu, go to " . l('Site building -> Blocks', 'admin/build/block') ." in the admin section. There you can place a Megamenu, most likely in the header region, and it will inherit its structure from the associated Drupal menu"),
  );

  foreach (_megamenu_menulist() as $menu) {
    $menu_details = menu_load($menu);
    _megamenu_verify_menu_entry($menu_details['menu_name']);
    $enabled = _megamenu_is_enabled($menu_details['menu_name']);

    $form['enabled'][$menu_details['menu_name']] = array(
        '#title' => t('Enabled'),
        '#type' => 'checkbox',
        '#default_value' => $enabled ? $enabled : 0,
    );
  }

  $form['save_configuration'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
  );

  $form['#theme'] = 'megamenu_admin';
  $form['#validate'][] = 'megamenu_admin_form_validate';
  $form['#submit'][] = 'megamenu_admin_form_submit';

  return $form;
}

function megamenu_settings_form(&$node, $form_state = null, $menu_name = '') {
  drupal_set_title('Mega-Menu Settings for: '.$menu_name);

  $form = array();

  $form['orientations'] = array(
    '#title' => t('Orientations'),
    '#type' => 'fieldset',
    '#description' => t('Set the orienation of various menu elements'),
  );

  $form['orientations']['menu_orientation'] = array(
    '#title' => t('Menu Orientation'),
    '#type' => 'radios',
    '#options' => array('horizontal' => 'horizontal', 'vertical' => 'vertical'),
    '#default_value' => _megamenu_get_menu_orientation_by_name($menu_name),
    '#description' => t('Select whether the mega menu will extend horizontally and drop down, or if it will extend vertically and fly out (experimental).'),
  );

  $form['orientations']['slot_orientation'] = array(
    '#title' => 'Slot Orientation',
    '#type' => 'radios',
    '#options' => array('columnar' => 'columnar', 'stacking' => 'stacking'),
    '#default_value' => _megamenu_get_slot_orientation_by_name($menu_name),
    '#description' => t('Select whether slots will sit next to each other (columnar) or stack on top of each other (stacking).'),
  );

  $form['style'] = array(
    '#title' => 'Style Settings',
    '#type' => 'fieldset',
    '#description' => t('Select a skin for this menu'),
  );

  // TODO: Create a function to return a list of default skins
  $form['style']['skin_options'] = array(
    '#title' => t('Skin Type'),
    '#type' => 'radios',
    '#options' => array('supplied_skin' => t('Use a skin supplied with this module'), 'custom_skin' => t('Use your own custom skin')),
    '#default_value' => _megamenu_is_skin_default($menu_name) ? 'supplied_skin' : 'custom_skin',
    '#description' => t('Select if you wish to use a pre-defined skin'),
  );

  $form['style']['default_skin'] = array(
     '#title' => t('Supplied Skin Name'),
      '#type' => 'select',
      '#options' => array('friendly' => 'friendly', 'minimal' => 'minimal'),
      '#default_value' => _megamenu_is_skin_default($menu_name) ?  _megamenu_get_skin_by_name($menu_name) : 'minimal',
      '#description' => t('Select one of the supplied skins to use'),
  );
  //TODO: phase out oldpath helper and fix core theme issue 
  //TODO: make the "file xists/does not" text stand out
  $form['style']['custom_css'] = array(
    '#type' => 'item',
    '#title' => t('Custom CSS File'),
    '#value' => '<p>'. t('You can add your own custom CSS for extending Megamenu at %csspath, either in the theme&rsquo;s existing CSS or as a separate file specified in the theme&rsquo;s .info file.  You must add custom CSS to your theme if you plan on using a custom skin.', array('%csspath'=> drupal_get_path('theme', variable_get('theme_default', NULL)))) .'</p>' . (file_exists(file_directory_path() .'/megamenu/megamenu-custom.css') ? '<p>'. t('WARNING: A file exists in the old custom CSS location of %oldpath, please move any custom CSS to the location specified above', array('%oldpath' => file_directory_path() .'/megamenu/megamenu-custom.css')) .'</p>':'') . (dirname(drupal_get_path('theme', variable_get('theme_default', NULL)))=='themes'  ? '<p>'. t('WARNING: you are using a core drupal theme, it is suggested that you duplicate your theme in sites/all/themes before adding custom CSS. For more information read &lsquo;Intermediate Theming Options&rsquo; at: ') . l('Drupal.org: Theming Options','http://drupal.org/node/805968') . '</p>':''), 
  );
  
    

  
  $form['style']['custom_skin'] = array(
    '#title' => t('Custom Skin Name'),
    '#type' => 'textfield',
    '#default_value' => _megamenu_is_skin_default($menu_name) ?  '' : _megamenu_get_skin_by_name($menu_name),
    '#description' => t('Type in the name of your custom skin (This will become a class value applied to the menu: megamenu-skin-[skin name]), and should be referenced in your custom CSS.'),
  );

  $form['save_configuration'] = array(
    '#type'=> 'submit',
    '#value' => t('Save Configuration'),
  );

  // Value placeholder
  $form['menu_name'] = array(
    '#type' => 'value',
    '#value' => $menu_name,
  );

  return $form;
}

/**
 * Validate the main admin form
 */
function megamenu_admin_form_validate($form, &$form_state) {
  //drupal_set_message(t('Validated'));

  // TODO: Is there anything to validate on this form???
}

/**
 *
 * Submit the main admin form
 */
function megamenu_admin_form_submit($form, &$form_state) {
  drupal_set_message(t('Configuration Saved'));

  foreach (_megamenu_menulist() as $menu) {
      db_query("UPDATE {megamenu} SET enabled = %d WHERE menu_name = '%s'", $form_state['values'][$menu], $menu);
  }
}


/**
 * Validate the settings form
 */
function megamenu_settings_form_validate($form, &$form_state) {
  $settings = $form_state['values']['settings'];

  if ($form_state['values']['skin_options'] == 'custom_skin') {
    if ($form_state['values']['custom_skin'] == '') {
      form_set_error('custom_skin', t('If you want to use a custom skin, you must specify its name'));
    }
    else if (!check_plain($form_state['values']['custom_skin'])) {
      form_set_error('custom_skin', t('Value must be plain text'));
    }
    else {
      $skin = $form_state['values']['custom_skin'];
    }

  }
  else {
    $skin = $form_state['values']['default_skin'];
  }

  $form_state['megamenu']['skin'] = $skin;
  
}

/**
 * Submit the settings form
 */
function megamenu_settings_form_submit($form, &$form_state) {
  $menu_name = $form_state['values']['menu_name'];
  $skin = $form_state['megamenu']['skin'];
  $menu_orientation = $form_state['values']['menu_orientation'];
  $slot_orientation = $form_state['values']['slot_orientation'];

  db_query("UPDATE {megamenu}
            SET
              menu_orientation = '%s',
              slot_orientation = '%s',
              skin = '%s'
            WHERE
              menu_name = '%s'", $menu_orientation, $slot_orientation, $skin, $menu_name);

}
