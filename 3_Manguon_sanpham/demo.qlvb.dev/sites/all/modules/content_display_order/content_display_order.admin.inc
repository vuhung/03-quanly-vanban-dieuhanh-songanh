<?php
// $Id: content_display_order.admin.inc,v 1.1 2010/04/30 13:09:17 skilip Exp $

/**
 * @file
 * Administrative interface for content type display order.
 */

/**
 * Menu callback; presents a listing of fields display settings for a content type.
 *
 * Form includes form widgets to select which fields appear for teaser, full node
 * and how the field labels should be rendered.
 */
function content_display_order_form(&$form_state, $type_name, $contexts_selector = 'basic') {
  module_load_include('inc', 'content', 'includes/content.admin');
  content_inactive_message($type_name);

  // Gather type information.
  $type = content_types($type_name);
  $field_types = _content_field_types();
  $fields = $type['fields'];
  $extra = $type['extra'];
  $max_weight = 0;

  $extra_values = variable_get("content_extra_display_order_$type_name", array());

  $groups = array();
  if (module_exists('fieldgroup')) {
    $groups = fieldgroup_groups($type['type']);
    $group_options = _fieldgroup_groups_label($type_name);
    $group_defaults = array();
  }
  $contexts = content_build_modes($contexts_selector);

  $form = array(
    '#tree' => TRUE,
    '#type_name' => $type['type'],
    '#fields' => array_keys($fields),
    '#groups' => array_keys($groups),
    '#contexts' => $contexts_selector,
    '#prefix' => '<div id="content-display-overview-form">', // We need to fake this in order to get the appropriate CSS styles from content-module.css 
    '#suffix' => '</div>',
  );

  if (empty($fields)) {
    drupal_set_message(t('There are no fields configured for this content type. You can add new fields on the <a href="@link">Manage fields</a> page.', array(
      '@link' => url('admin/content/node-type/'. $type['url_str'] .'/fields'))), 'warning');
    return $form;
  }

  foreach (array_keys($contexts) as $context) {
    $form[$context] = array();

    foreach ($groups as $name => $group) {
      $defaults = $group['settings']['display'];
      $weight = (isset($defaults[$context]['weight']) ? $defaults[$context]['weight'] : $defaults['weight']);

      foreach (array_keys($group['fields']) as $field_name) {
        $group_defaults[$field_name] = $name;
      }

      $form[$context][$name] = array(
        'human_name' => array('#value' => check_plain($group['label'])),
        'weight' => array('#type' => 'textfield', '#default_value' => $weight, '#size' => 3, '#attributes' => array('class' => 'field-weight')),
        'parent' => array('#type' => 'hidden', '#default_value' => '', '#attributes' => array('class' => 'group-parent')),
        'hidden_name' => array('#type' => 'hidden', '#default_value' => $name, '#attributes' => array('class' => 'field-name')),
        '#root' => TRUE,
        '#leaf' => FALSE,
        '#row_type' => 'group',
      );
    }

    // Non-CCK 'fields'.
    foreach ($extra as $name => $label) {
      if ($extra[$name]['view']) {
        $weight = ($extra_values[$context] && isset($extra_values[$context][$name]) ? $extra_values[$context][$name] : $extra[$name]['weight']);
        $max_weight = ($weight > $max_weight ? $weight : $weight);

        $form[$context][$name] = array(
          'human_name' => array('#value' => check_plain(t($extra[$name]['label']))),
          'weight' => array('#type' => 'textfield', '#default_value' => $weight, '#size' => 3, '#attributes' => array('class' => 'field-weight')),
          'hidden_name' => array('#type' => 'hidden', '#default_value' => $name, '#attributes' => array('class' => 'field-name')),
          '#root' => TRUE,
          '#leaf' => TRUE,
          '#disabled_row' => TRUE,
          '#row_type' => 'extra',
        );

        if (!empty($groups)) {
          $form[$context][$name]['parent'] = array('#type' => 'select', '#default_value' => '', '#options' => $group_options, '#attributes' => array('class' => 'group-parent'));
        }
        else {
          $form[$context][$name]['parent'] = array('#type' => 'hidden', '#default_value' => '', '#attributes' => array('class' => 'group-parent'));
        }

        if (!in_array($name, $form['#fields'])) {
          $form['#fields'][] = $name;
        }
      }
    }

    // CCK 'fields'.
    foreach ($fields as $name => $field) {
      $field_type = $field_types[$field['type']];
      $defaults = $field['display_settings'];
      $weight = (isset($defaults[$context]['weight']) ? $defaults[$context]['weight'] : $max_weight);
      $parent = (isset($defaults[$context]['parent']) ? $defaults[$context]['parent'] : $group_defaults[$name]);

      $form[$context][$name] = array(
        'human_name' => array('#value' => check_plain($field['widget']['label'])),
        'weight' => array('#type' => 'textfield', '#default_value' => $weight, '#size' => 3, '#attributes' => array('class' => 'field-weight')),
        'hidden_name' => array('#type' => 'hidden', '#default_value' => $name, '#attributes' => array('class' => 'field-name')),
        '#root' => FALSE,
        '#leaf' => TRUE,
        '#row_type' => 'field',
      );

      if (!empty($groups)) {
        $form[$context][$name]['parent'] = array('#type' => 'select', '#default_value' => $parent, '#options' => $group_options, '#attributes' => array('class' => 'group-parent'));
        $form[$context][$name]['human_name']['#depth'] = ($parent ? 1 : 0);
      }
      else {
        $form[$context][$name]['parent'] = array('#type' => 'hidden', '#default_value' => '', '#attributes' => array('class' => 'group-parent'));
      }
    }
  }

  $form['submit'] = array('#type' => 'submit', '#value' => t('Save'));
  return $form;
}

/**
 * Submit handler for the display overview form.
 */
function content_display_order_form_submit($form, &$form_state) {
  module_load_include('inc', 'content', 'includes/content.crud');

  $type_name = $form['#type_name'];
  $extra = variable_get("content_extra_display_order_$type_name", array());

  if (module_exists('fieldgroup')) {
    $groups = fieldgroup_groups($type_name);
  }

  foreach (array_keys(content_build_modes($form['#contexts'])) as $context) {
    foreach ($form_state['values'][$context] as $field_name => $values) {
      switch ($form[$context][$field_name]['#row_type']) {
        case 'field':
          if ($field = content_fields($field_name, $type_name)) {
            $field['display_settings'][$context]['weight'] = $values['weight'];
            $field['display_settings'][$context]['parent'] = $values['parent'];
            content_field_instance_update($field);
          }
          break;
        case 'group':
          if ($groups && $groups[$field_name]) {
            $groups[$field_name]['settings']['display'][$context]['weight'] = $values['weight'];
            fieldgroup_save_group($type_name, $groups[$field_name]);
          }
          break;
        case 'extra':
          $extra[$context][$field_name] = $values['weight'];
          break;
        default:
          break;
      }
    }
  }

  if (!empty($extra)) {
    variable_set("content_extra_display_order_$type_name", $extra);
  }
  else {
    variable_del("content_extra_display_order_$type_name");
  }
  drupal_set_message(t('Your settings have been saved.'));
}

/**
 * Theme function for the content_display_order_settings_form
 */
function theme_content_display_order_form($form) {
  foreach (content_build_modes($form['#contexts']) as $context => $context_data) {
    $header = array($context_data['title'], '');
    $rows = array();
    $id = "content-display-order-$context";
    $order = _content_overview_order($form[$context], $form['#fields'], $form['#groups']);

    drupal_add_tabledrag($id, 'match', 'parent', 'group-parent', 'group-parent', 'field-name', TRUE, 1);
    drupal_add_tabledrag($id, 'order', 'sibling', 'field-weight');

    foreach ($order as $key) {
      $element = &$form[$context][$key];

      $label_class = in_array($key, $form['#groups']) ? 'label-group' : 'label-field';
      $indentation = theme('indentation', $element['human_name']['#depth'] ? 1 : 0);
      $row_class = 'draggable';
      $row_class .= $element['#disabled_row'] ? ' menu-disabled' : '';
      $row_class .= $element['#add_new'] ? ' content-add-new' : '';
      $row_class .= $element['#leaf'] ? ' tabledrag-leaf' : '';
      $row_class .= $element['#root'] ? ' tabledrag-root' : '';

      $rows[] = array(
        'data' => array(
          $indentation .'<span class="'. $label_class .'">'. drupal_render($element['human_name']) .'</span>',
          drupal_render($element['weight']) . drupal_render($element['parent']) . drupal_render($element['hidden_name']),
        ),
        'class' => $row_class,
      );
    }
    $form["$context-view"] = array('#value' => theme('table', $header, $rows, array('id' => $id)), '#weight' => -10);
  }
  $form['submit-view'] = array('#value' => drupal_render($form['submit']));
  return drupal_render($form);
}
