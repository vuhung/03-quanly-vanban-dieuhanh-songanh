<?php

function basic_webmail_mail_batch_import() {
    $items = _basic_webmail_check_internal_email();
    $counter = count($items);
    drupal_set_title(t('Trình tạo văn bản đến từ Email'));
    $output = '';
    if ($counter) {
        return $output . drupal_get_form('_basic_webmail_mail_batch_import_form', $counter);
    }
    $output.=t("<h2>Hiện tại không có email nào đến từ hệ thống</h2>");
    $output.=l('Click vào đây', 'basic_webmail');
    $output.=t(" để quay trở lại hệ thống mail");
    return $output;
}

function _basic_webmail_mail_batch_import_form(&$form_state, $counter) {
    $info = t('Hiện tại đang có @counter email đến từ hệ thống', array('@counter' => $counter));
    $form['info'] = array(
        '#prefix' => '<div class="messages warning">',
        '#suffix' => '</div>',
        '#type' => 'markup',
        '#value' => $info,
    );
    $form['nofity'] = array(
        '#type' => 'fieldset',
        '#title' => t('Lưu ý'),
    );
    $max_emails_allow = variable_get('basic_webmail_number_imports', 20);
    $message = "- Trình tạo văn bản sẽ chỉ tạo văn bản từ những email được gửi được gửi bởi cùng hệ thống phần mềm, nếu bạn muốn tạo từ những email khác vui lòng vào từng email để chọn 'Tạo văn bản đến'<br>";
    $message.="- Trình tạo văn bản sẽ tạo 1 lần tối đa $max_emails_allow văn bản từ email, nếu bạn muốn thay đổi xin liên hệ với nhà sản xuất phần mềm<br>";
    $form['nofity']['message'] = array(
        '#prefix' => '<div>',
        '#suffix' => '</div>',
        '#type' => 'markup',
        '#value' => $message,
    );
    $form['start_import'] = array(
        '#type' => 'markup',
        '#value' => 'Để bắt đầu tạo hàng loạt văn bản đến , click vào đây ',);
    $form['submit'] = array(
        '#type' => 'submit',
        '#value' => 'Tạo hàng loạt văn bản đến',
    );
    return $form;
}

function _basic_webmail_mail_batch_import_form_submit(&$form_state) {
    $items = _basic_webmail_check_internal_email();
    $counter = count($items);
    if (!$counter) {
        drupal_set_message(t('Hiện tại không có email nào từ hệ thống.'), 'error');
        return;
    }
    $batch = array(
        'operations' => array(),
        'finished' => 'basic_webmail_batch_finished',
        'title' => t('Trình tạo văn bản đến từ Email'),
        'init_message' => t('Bắt đầu ...'),
        'progress_message' => t('Hoàn thành: @percentage %'),
        'error_message' => t('Có lỗi xảy ra trong quá trình tạo văn bản đến từ Email.'),
        'file' => drupal_get_path('module', 'basic_webmail') . '/basic_webmail_batch_import.inc',
    );
    $batch['operations'][] = array('_basic_webmail_mail_import_one_mail', array($items));
    batch_set($batch);
}

function _basic_webmail_mail_import_one_mail($items = array(), &$context) {
    Global $user;
    //xóa email sau khi import
    $action_after_import = variable_get("basic_webmail_delete_after_import", 1);
    $limit = 1;
    if (!isset($context['sandbox']['progress'])) {
        $context['sandbox']['progress'] = 0;
        $max_emails_allow = variable_get('basic_webmail_number_imports', 20);
        $context['sandbox']['max'] = (count($items) > $max_emails_allow) ? $max_emails_allow : count($items);
    }
    if (!isset($context['sandbox']['items'])) {
        $context['sandbox']['items'] = $items;
    }
    //being saving email to node		 
    if (!empty($context['sandbox']['items'])) {
        if ($context['sandbox']['progress'] != 0) {
            array_splice($context['sandbox']['items'], 0, $limit);
        }
        $mid = $context['sandbox']['items'][0];
        //create new vbden 
        $obj = _basic_webmail_open_connect();
        if (!$obj) {
            drupal_set_message('Không kết nối được với IMAP server', 'error');
            $context['sandbox']['items'] = array();
            return;
        }
        //chi import tu dong nhung mail tu INBOX		 
        $message = basic_webmail_get_one_message($mid, 1, $obj);
        if (!$message) {
            $last_error = imap_last_error();
            watchdog('basic_webmail', 'There was an error importing the message : !last_error', array('!last_error' => $last_error), WATCHDOG_ERROR);
        } else {
            $email = $message['fromaddress']->mailbox . '@' . $message['fromaddress']->host;
            $name = $message['fromaddress']->personal;
            $node = _basic_webmail_in_system($message['html'][0], $email);
            $node['email'] = $email;
            $node['name'] = _basic_webmail_decode_mime_str($name);
            if ($node) {
                $nid = basic_webmail_create_vbd($node, $message['attachments']);
                if ($nid) {
                    // Nếu xóa Email
                    if ($action_after_import != 1) {
                        $obj->delete($mid);
                    }
                    $context['results']['emails']++;
                    $context['sandbox']['mid'][] = $mid;
                }
            } else {
                $fake_message = variable_get('basic_webmail_fake_messages', array());
                $fake_message[] = $mid;
                variable_set(basic_webmail_fake_messages, $fake_message);
            }
        }
        //end of create new vbden		
        $context['sandbox']['progress']++;
        $context['message'] = t('Đã import %email email trên tổng số %count email', array('%email' => $context['sandbox']['progress'], '%count' => $context['sandbox']['max']));
    }
    if ($context['sandbox']['progress'] != $context['sandbox']['max']) {
        $context['finished'] = $context['sandbox']['progress'] / $context['sandbox']['max'];
    } else {
        // mảng ID các Email đã được Import
        $mid = $context['sandbox']['mid'];
        if ($action_after_import != 1 && !empty($mid)) {
            // Nếu chỉ xóa vào thùng rác
            if ($action_after_import == 2) {
                $mailbox = basic_webmail_get_mailbox();
                $destination = mb_convert_encoding($mailbox[4]['fullname'], 'UTF7-IMAP', 'UTF-8');
                if (!$obj->get_status($destination)) {
                    if (!$obj->add_folder($destination)) {
                        drupal_set_message('Hộp thư Thùng rác không tồn tại, tuy nhiên hệ thống cố gắng tạo hộp thư này trên máy chủ IMAP không thành công', 'error');
                    }
                }
                if (!$obj->move(implode(',', $mid), $destination)) {
                    drupal_set_message('Có lỗi khi chuyển thư tới thùng giác', 'error');
                }
            }
            // Xóa email đã import trong database
            $placeHolder = db_placeholders($mid, 'int');
            $mid[] = $user->uid;
            $mid[] = 1;
            if (!db_query("DELETE FROM {basic_webmail_messages}  WHERE mid IN($placeHolder) AND user_id = %d AND mboxid = %d", $mid)) {
                drupal_set_message('Có lỗi khi cập nhật vào cơ sở dữ liệu', 'error');
            }
            //execute delete mail
            $obj->expunge();
        }
    }
}

function basic_webmail_batch_finished($success, $results, $operations) {
    if ($success) {
        // Here we do something meaningful with the results.
        if ($results['emails']) {
            $message = t('Đã tạo @mail văn bản đến từ email', array('@mail' => $results['emails']));
            drupal_set_message($message);
        }
    } else {
        // An error occurred.
        // $operations contains the operations that remained unprocessed.
        $error_operation = reset($operations);
        $message = t('An error occurred while processing %error_operation with arguments: @arguments', array('%error_operation' => $error_operation[0], '@arguments' => print_r($error_operation[1], TRUE)));
        watchdog('basic_webmail_mail', 'An error occurred while processing %error_operation with arguments: @arguments', array('%error_operation' => $error_operation[0], '@arguments' => print_r($error_operation[1], TRUE)), WATCHDOG_ERROR);
        drupal_set_message($message, "error");
    }
}