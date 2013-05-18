<?php
// $Id: views_table_highlighter_plugin_style.inc,v 1.1.2.1 2010/01/20 23:19:44 smokris Exp $

class views_table_highlighter_plugin_style extends views_plugin_style_table {
  function options_form(&$form, &$form_values) {
    parent::options_form($form, $form_values);
    $vth_options = $this->options['views_table_highlighter'];

    // prepare some sample values for our form, by executing the current view.
    $example_view = $this->view->copy();
    $example_view->execute();
    $fieldtext = '';
    $fields = (array)$example_view->result[0];
    ksort($fields);
    foreach($fields as $field => $value)
      $fieldtext .= '<li>$'.$field.' == "'.htmlentities(addslashes($value)).'"</li>';
    reset($fields);

    $form['views_table_highlighter'] = array(
      '#type' => 'fieldset',
      '#title' => t('Table Highlighter'),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      'code' => array(
        '#type' => 'textarea',
        '#rows' => 2,
        '#title' => t('Highlighter PHP code'),
        '#default_value' => $vth_options['code'] ? $vth_options['code'] : 'if ($'.key($fields).'=="'.htmlentities(current($fields)).'") return \'red\';',
        '#process' => array('views_process_dependency'),
        '#description' => "<p>".t('Enter PHP code that returns the color with which this row should be highlighted.  Return without a value to leave this row unlit.').'</p>'
          .'<table><tr valign="top"><td>'
            .t('Available fields:')
            .'<ul>'.$fieldtext.'</ul>'
            .t('(Above are sample values from the first row of this view.)')
          .'</td><td>'
            .t('Available colors:')
            ."<ul><li>'red'</li><li>'yellow'</li><li>'green'</li><li>'cyan'</li><li>'blue'</li><li>'magenta'</li></ul>"
          .'</td></tr></table>'
      ),
    );
  }
}

