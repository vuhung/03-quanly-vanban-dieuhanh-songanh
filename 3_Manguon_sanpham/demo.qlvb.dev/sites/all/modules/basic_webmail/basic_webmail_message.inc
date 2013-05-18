<?php

/**
 * Enter description here ...
 * @param unknown_type $mid
 * @param unknown_type $fid
 * @author truongnx.ithut@gmail.com
 */
function basic_webmail_message_detail($mid, $fid) {
    Global $user;
    if (arg(0) == 'basic_webmail' && arg(1) == 'message' && $mid == '') {
        drupal_set_message(t('No message_number was given when attempting to view a message.'), 'error');
        return;
    }
    basic_webmail_add_js();
    $message = basic_webmail_get_one_message($mid, $fid);
    if (!$message) {
        return '';
    }
    //update read
    db_query("UPDATE {basic_webmail_messages} SET  mread=1 WHERE mid =%d AND user_id = %d AND mboxid = %d", $mid, $user->uid, $fid);
    // Set the title of the page. 
    if (!$message ['subject']) {
        $message ['subject'] = '(Không có chủ đề)';
    }
    $title = check_plain($message ['subject']);
    drupal_set_title(t('!title', array('!title' => $title)));
    $output .='<div class="bwm_detail_header">';
    //Show sender
    $output .= '<div class="bwm_detail_sender">';
    $sender_email = $message ['fromaddress']->mailbox . '@' . $message ['fromaddress']->host;
    $sender_personal = _basic_webmail_decode_mime_str($message ['fromaddress']->personal);
    if ($sender_personal) {
        $output .= "Người gửi:" . l($sender_personal, "basic_webmail/sendmail/$sender_personal <$sender_email>");
    } else {
        $output .= "Người gửi:" . l($sender_email, 'basic_webmail/sendmail/' . $sender_email);
    }
    $output .= '</div>';

    //show ngay gui

    $output .= '<div class="bwm_detail_date">';
    $tmp = explode(' ', trim($message ['date']));
    $message_date = _basic_webmail_get_timestamp($tmp);
    $output .= 'Ngày gửi : ' . date(' d/m/Y  h\h:m\':s\'\'', $message_date);
    $output .= '</div>';

    //show nguoi nhan	
    $to = array();
    if (!empty($message ['toaddress'])) {
        foreach ($message ['toaddress'] as $item) {
            $to_email = $item->mailbox . '@' . $item->host;
            $to_personal = _basic_webmail_decode_mime_str($item->personal);
            if ($to_personal) {
                $to [] = l($to_personal, "basic_webmail/sendmail/$to_personal  <$to_email>");
            } else {
                $to [] = l($to_email, "basic_webmail/sendmail/$to_email");
            }
        }
    }

    $output .= '<div class="bwm_detail_to">';
    $output .= 'Người nhận : ' . implode(',', $to);
    $output .= '</div>';

    //show cc	
    if (count($message ['ccaddress'])) {
        $cc = array();
        foreach ($message ['ccaddress'] as $item) {
            $cc_email = $item->mailbox . '@' . $item->host;
            $cc_personal = _basic_webmail_decode_mime_str($item->personal);
            if ($to_personal) {
                $cc [] = l($cc_personal, "basic_webmail/sendmail/$cc_personal  <$cc_email>");
            } else {
                $cc [] = l($cc_email, "basic_webmail/sendmail/$cc_email");
            }
        }
        $output .= '<div class="bwm_detail_cc">';
        $output .= 'CC : ' . implode(',', $cc);
        $output .= '</div>';
    }
    $output .= '</div>'; //end of header
    //show content
    $output .= '<div class="bwm_detail_content">';
    if (count($message ['html'])) {
        //show html
        $output .= '<div class="bwm_detail_html">';
        $output .= implode('<hr width="50%">', $message ['html']);
        $output .= '</div>';
    } elseif (count($message ['plain'])) {
        //show plain text
        $output .= '<div class="bwm_detail_text">';
        $output .= check_plain(implode('<hr width="50%">', $message ['plain']));
        $output .= '</div>';
    }
    //show attachments		
    if ($message ['attachments']) {
        $output .= '<div class="bwm_detail_attach">';
        $attachment_links = array();
        $output .= 'File đính kèm  : ' . count($message ['attachments']) . ' file ';
        foreach ($message ['attachments'] as $item) {
            $attachment_links [] = l(basename($item), _basic_webmail_file_create_url($item));
        }
        $output .= theme_item_list($attachment_links);
        $output .= '</div>';
    }
    $output .= '</div>'; //end of content
    return $output;
}

function basic_webmail_message($mid = '', $fid = 1) {
    //check whether message_number is avaiable or not	
    $output .= '<div class="basic_webmail_main">';
    $output .= '<div class="basic_webmail_navigate">';
    $output .= '<div class="basic_mail_compose_mail">' . l('Soạn', 'basic_webmail/sendmail') . '</div>';
    $output .= '<div class="basic_mail_folder_list" id="bwmfdl"></div>';
    $output .= '</div>';
    $output .= '<div class="basic_webmail_mail_list">';
    $output .= '<div>';
    $output .= drupal_get_form('_basic_webmail_message_form', $fid);
    $output .= '</div>';
    $output .= '<div>';
    $output .= basic_webmail_message_detail($mid, $fid);
    $output .= '</div>';
    $output .= '</div>';
    $output .= '</div>';
    ////add js 
    $mailbox = basic_webmail_get_mailbox();
    $mailCounter = _basic_webmail_count_new($mailbox);
    $setting = array(
        'basePath' => base_path(),
        'mid' => $mid,
        'fid' => $fid,
        'mailbox' => array_values($mailbox),
        'mailCounter' => $mailCounter);
    drupal_add_js(array('basic_webmail' => $setting), 'setting');
    drupal_add_js(drupal_get_path('module', 'basic_webmail') . '/js/basic_webmail_message.js');
    //end of add js
    return $output;
}

//  End of _basic_webmail_message().
/**
 * 
 * Enter description here ...
 * @param unknown_type $form_state
 * @param unknown_type $fid
 */
function _basic_webmail_message_form(&$form_state, $fid) {
    $mailbox = basic_webmail_get_mailbox();
    $form ['import'] = array('#type' => 'submit', '#name' => 'import', '#value' => t('Tạo văn bản đến '), '#prefix' => '<div class="container-inline">');
    $form ['reply'] = array('#type' => 'button', '#name' => 'reply', '#value' => t('Trả lời'));
    $form ['forward'] = array('#type' => 'button', '#name' => 'forward', '#value' => t('Chuyển tiếp thư'));
    $form ['delete'] = array('#type' => 'submit', '#name' => 'delete', '#value' => t('Xóa thư'));
    $form ['move'] = array('#type' => 'markup', '#value' => '  Chuyển tới');

    $option = array();
    foreach ($mailbox as $item) {
        if ($item['parent'] == -1 || $item['id'] == $fid) {
            continue;
        }
        $option[$item['id']] = $item['alias'];
    }
    $form ['folder_name'] = array('#type' => 'select', '#options' => $option);
    $form ['move_to'] = array('#type' => 'submit', '#name' => 'move_to', '#value' => t('Thực hiện'), '#suffix' => '</div>');

    return $form;
}

function _basic_webmail_file_create_url($path) {
    // Strip file_directory_path from $path. We only include relative paths in urls.
    if (strpos($path, file_directory_path() . '/') === 0) {
        $path = trim(substr($path, strlen(file_directory_path())), '\\/');
    }
    return $GLOBALS['base_url'] . '/' . file_directory_path() . '/' . str_replace('\\', '/', $path);
}

?>