<?php 
/**
 * Administration settings form.
 *
 * @return
 *   The completed form definition.
 */
function basic_webmail_admin_settings() {
  $form = array();

  // Cấu hình chung
  $form['basic_webmail_general_settings'] = array(
    '#type'        => 'fieldset',
    '#title'       => 'Cấu hình chung',
    '#collapsible' => TRUE,
    '#collapsed'   => FALSE,
  );
  $form['basic_webmail_general_settings']['basic_webmail_messages_per_page'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Số lượng Email '),
    '#size'          => 10,
    '#default_value' => variable_get('basic_webmail_messages_per_page', 25),
    '#description'   => t('Số lượng Email sẽ hiển thị trên 1 trang khi xem danh sách Email'),
  );
  $form['basic_webmail_general_settings']['basic_webmail_subject_characters'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Số lượng ký tự cho phép'),
    '#size'          => 10,
    '#default_value' => variable_get('basic_webmail_subject_characters', 40),
    '#description'   => t('Đây là số ký tự của tiêu đề Email sẽ được hiển thị khi xem danh sách các Email'),
                          
  );    
  $form['basic_webmail_general_settings']['basic_webmail_attachment_location'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Thư mục lưu file đính kèm'),
    '#default_value' => variable_get('basic_webmail_attachment_location', file_directory_path() . '/attachments'),
    '#description'   => t('Thư mục lưu file đính kèm của email.'),
  ); 
  // Cấu hình trạng thái gửi văn bản
  $form['basic_webmail_outgoingdoc_config'] = array(
    '#type'        => 'fieldset',
    '#title'       => 'Cấu hình gửi văn bản đi',
    '#collapsible' => TRUE,
    '#collapsed'   => FALSE,
  );
  $form['basic_webmail_outgoingdoc_config']['basic_webmail_email_name'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Tên của đơn vị'),
    '#default_value' => variable_get('basic_webmail_email_name', ''),
    '#description'   => t("Đây tên của đơn vị sẽ hiển thị cùng với Email khi gửi văn bản bằng Email"),
  );
  $form['basic_webmail_outgoingdoc_config']['basic_webmail_default_domain'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Tên miền mặc định của địa chỉ Email'),
    '#default_value' => variable_get('basic_webmail_default_domain', ''),
    '#description'   => t("Tên miền mặc định cho Email nhập không cần thêm hậu tố @domain.com"),
  );
  $form['basic_webmail_outgoingdoc_config']['basic_webmail_wfsate_sendmail'] = array(
    '#type'          => 'radios',
    '#title'         => t('Trạng thái của văn bản đi được phép gửi mail'),
    '#size'          => 10,
    '#default_value' => variable_get('basic_webmail_wfsate_sendmail', 19),
    '#options' 	     => workflow_get_states(workflow_get_workflow_for_type('004_vanbandi')),
  );
  
  $form['basic_webmail_outgoingdoc_config']['basic_webmail_save_after_sent'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Lưu lại văn bản đã gửi qua Email'),
    '#default_value' => variable_get('basic_webmail_save_after_sent', 0),
    '#description'   => t('Văn bản đã gửi qua Email lưu lại Hộp thư đi'),
  );
  
  // Cấu hình tạo văn bản đến từ Email
  $form['basic_webmail_import_email'] = array(
    '#type'        => 'fieldset',
    '#title'       => 'Cấu hình tạo văn bản đến từ Email',
    '#collapsible' => TRUE,
    '#collapsed'   => FALSE,
  );
  $form['basic_webmail_import_email']['basic_webmail_number_imports'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Số lượng Email tạo văn bản'),
    '#size'          => 10,
    '#default_value' => variable_get('basic_webmail_number_imports', 20),
    '#description'   => t("Số lượg tối đa của Email cho trong 1 lần tạo hàng loạt các văn bản từ Email. "),
  );
  $form['basic_webmail_import_email']['basic_webmail_delete_after_import'] = array(
    '#type'          => 'radios',
    '#title'         => t('Sau khi tạo văn bản đến từ Email'),
    '#size'          => 10,
    '#default_value' => variable_get('basic_webmail_delete_after_import', 1),
    '#options' 	     => array(
                        1=>"Không xóa Email",
                        2=>"Xóa Email nhưng vẫn lưu trong thùng rác",
                        3=>"Xóa Email khỏi hộp thư điện tử"
                        ),
    '#description'   => t("Hành động sẽ thực hiện với Email sau khi đã tạo văn bản tư Email đó vào hệ thống."),
  );
  
  // cấu hình cho imap server
  $form['basic_webmail_imap_settings'] = array(
    '#type'        => 'fieldset',
    '#title'       => t('Imap Server settings'),
    '#collapsible' => TRUE,
    '#collapsed'   => FALSE,
  );
  $form['basic_webmail_imap_settings']['basic_webmail_imap_server'] = array(
    '#type'          => 'textfield',
    '#title'         => t('IMAP Server address'),
    '#default_value' => variable_get('basic_webmail_imap_server', ''),
    '#description'   => t('The address to the imap server you wish to connect to.'),
  );
  $form['basic_webmail_imap_settings']['basic_webmail_imap_port'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Connection port'),
    '#size'          => 10,
    '#default_value' => variable_get('basic_webmail_imap_port', 143),
    '#description'   => t('The default IMAP port is 143 if one is not specified.'),
  );
  $form['basic_webmail_imap_settings']['basic_webmail_secure_log_in'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Secure login.'),
    '#default_value' => variable_get('basic_webmail_secure_log_in', 0),
    '#description'   => t('Check to make a secure connection to your IMAP Server.'),
  );
  $form['basic_webmail_imap_settings']['basic_webmail_use_ssl'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Encrypt session using SSL.'),
    '#default_value' => variable_get('basic_webmail_use_ssl', 0),
    '#description'   => t('Use SSL to connect to the server.'),
  );
  $form['basic_webmail_imap_settings']['basic_webmail_validate_cert'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Validate certificate.'),
    '#default_value' => variable_get('basic_webmail_validate_cert', 0),
    '#description'   => t('When using a secure connection, validate the certificate.'),
  );
  $form['basic_webmail_imap_settings']['basic_webmail_use_tls'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Encrypt session using TLS.'),
    '#default_value' => variable_get('basic_webmail_use_tls', 0),
    '#description'   => t('Use TLS to connect to the server.'),
  );

  //smtp server config
  $form['basic_webmail_smtp_settings'] = array(
    '#type'        => 'fieldset',
    '#title'       => t('SMTP Server settings'),
    '#collapsible' => TRUE,
    '#collapsed'   => FALSE,
  );
  $form['basic_webmail_smtp_settings']['basic_webmail_smtp_server'] = array(
    '#type'          => 'textfield',
    '#title'         => t('SMTP Server address'),
    '#default_value' => variable_get('basic_webmail_smtp_server', ''),
    '#description'   => t('The address to the smtp server you wish to connect to.'),
  );
  $form['basic_webmail_smtp_settings']['basic_webmail_smtp_port'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Connection port'),
    '#size'          => 10,
    '#default_value' => variable_get('basic_webmail_smtp_port', 25),
    '#description'   => t('The default SMTP port is 25 if one is not specified.'),
  );
  return system_settings_form($form);
}// End of basic_webmail_admin_settings().


/**
 * Additional vaidation for the administration settings form.
 *
 * @param form_id
 *   The form ID of the passed form.
 * @param $form_state
 *   The form values which you may perform validation on.
 */
function basic_webmail_admin_settings_validate($form, &$form_state) {
  if (!file_check_directory($form_state['values']['basic_webmail_attachment_location'], FILE_CREATE_DIRECTORY)) {
    form_set_error('basic_webmail_attachment_location', t('The directory does not exist or is not writable, and there was a problem creating the path.'));
  }
  if ($form_state['values']['basic_webmail_use_ssl'] == 1 && $form_state['values']['basic_webmail_server_port'] == '143') {
    form_set_error('basic_webmail_server_port', t('The normal port for secure IMAP is 993.'));
  }
}  //  End of basic_webmail_admin_settings_validate().

?>