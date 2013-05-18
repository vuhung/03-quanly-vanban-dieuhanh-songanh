<?php

function basic_webmail_send_mail($type = 'new', $mid = '', $fid = '') {
    basic_webmail_add_js();
    $output .= '<div class="basic_webmail_main">';
    $output .= '<div class="basic_webmail_navigate">';
    $output .= '<div class="basic_mail_compose_mail">' . l('Soạn', 'basic_webmail/sendmail') . '</div>';
    $output .= '<div class="basic_mail_folder_list" id="bwmfdl"></div>';
    $output .= '</div>';
    $output .= '<div class="basic_webmail_mail_list">';
    $output .= '<div>';
    $output .= drupal_get_form('basic_webmail_send_mail_form', $type, $mid, $fid);
    $output .= '</div>';
    $output .= '<div>';
    $output .= '</div>';
    $output .= '</div>';
    $output .= '</div>';
    //add js
    $mailbox = basic_webmail_get_mailbox();
    $mailCounter = _basic_webmail_count_new($mailbox);
    $setting = array('mailbox' => array_values($mailbox), 'mailCounter' => $mailCounter);
    drupal_add_js(array('basic_webmail' => $setting), 'setting');
    drupal_add_js(drupal_get_path('module', 'basic_webmail') . '/js/basic_webmail_send.js');
    //end of add js
    return $output;
}

function basic_webmail_send_mail_form(&$form_state, $type = 'new', $mid = '', $fid = '') {
    $form = array();
    $form ['send'] = array('#type' => 'submit', '#name' => 'send', '#value' => t('Gửi thư'), '#prefix' => '<div class="container-inline">');
    $form ['cancel'] = array('#type' => 'button', '#name' => 'cancel', '#value' => t('Hoãn'));
    $form ['save'] = array('#type' => 'submit', '#name' => 'save', '#value' => t('Lưu bản nháp'), '#suffix' => '</div>');
    // Check to see if this is a reply or forward.
    $sendto = '';
    $subject = '';
    $body = '';
    $att = array();
    //incase of reply or forward or continute
    if ($type == 'reply' || $type == 'forward' || $type == 'continue') {
        $msg = basic_webmail_get_one_message($mid, $fid);
        if ($msg) {
            ////set body
            if ($msg['plain']) {
                foreach ($msg['plain'] as $item) {
                    $body[] = $item;
                }
                $body = implode('\n\r', $body);
            }
            if ($msg['fromaddress']->personal) {
                $sendto = _basic_webmail_decode_mime_str($msg['fromaddress']->personal);
                $sendto.='<' . $msg['fromaddress']->mailbox . '@' . $msg['fromaddress']->host . '>';
            } else {
                $sendto.=$msg['fromaddress']->mailbox . '@' . $msg['fromaddress']->host;
            };
            //set subject
            switch ($type) {
                case 'reply':
                    //set subject
                    $subject = 'Re: ' . $msg['subject'];
                    //set body
                    if ($body) {
                        $tmp = explode(' ', $msg['date']);
                        $date = _basic_webmail_get_timestamp($tmp);
                        $reply_mark = "\n\r\n\r\n\rVào lúc " . date("h\\hm' \\N\\gà\\y d/m/Y", $date) . " $sendto đã viết: \n\r";
                        $body = explode("\n", "\n" . trim($body));
                        $body = implode("\n>", $body);
                        $body = $reply_mark . $body;
                    }
                    break;
                case 'forward':
                    $tmp = explode(' ', $msg['date']);
                    $date = _basic_webmail_get_timestamp($tmp);
                    $forward_mark = "\n\r\n\r\n\r---------- Thư đã chuyển tiếp ----------\n";
                    $forward_mark.="Từ: $sendto \n";
                    $forward_mark.="Ngày: " . date("h\\hm' \\N\\gà\\y d/m/Y", $date) . " \n";
                    $forward_mark.="Chủ đề: " . $msg['subject'] . " \n";
                    //set to
                    $to = array();
                    foreach ($msg ['toaddress'] as $item) {
                        $to_email = $item->mailbox . '@' . $item->host;
                        $to_personal = _basic_webmail_decode_mime_str($item->personal);
                        if ($to_personal) {
                            $to [] = "$to_personal  <$to_email>";
                        } else {
                            $to [] = $to_email;
                        }
                    }
                    $forward_mark.="Đến: " . implode(",", $to) . "\n\r\n\r";
                    $body = $forward_mark . $body;
                    $subject = 'Fwd: ' . $msg['subject'];
                    //set sendto 
                    $sendto = '';
                    break;
                case 'continue':
                    $subject = $msg['subject'];
                    break;
                default:
                    break;
            }
            //set attach
            if (!empty($msg['attachments'])) {
                foreach ($msg['attachments'] as $item) {
                    $path = explode('/', $item);
                    $filename = array_pop($path);
                    $fileid = array_pop($path);
                    //create hidden field
                    $attid = md5($fileid . $filename);
                    $att[$attid] = $filename;
                    $form [$attid] = array('#type' => 'hidden', '#value' => $fileid . '/' . $filename);
                }
            }
        }
    }


    $form ['sendto'] = array('#prefix' => '<div class="dK">', '#suffix' => '</div>', '#type' => 'textfield', '#title' => 'Người nhận', '#default_value' => $sendto, '#maxlength' => 1024, '#autocomplete_path' => 'basic_webmail/autocomplete');
    $form ['sendcc'] = array('#prefix' => '<div class="dK">', '#suffix' => '</div>', '#type' => 'textfield', '#title' => t('CC'), '#default_value' => '', '#maxlength' => 1024, '#autocomplete_path' => 'basic_webmail/autocomplete');
    $form ['sendbcc'] = array('#prefix' => '<div class="dK">', '#suffix' => '</div>', '#type' => 'textfield', '#title' => t('BCC'), '#default_value' => '', '#maxlength' => 1024, '#autocomplete_path' => 'basic_webmail/autocomplete');
    $form ['addCc'] = array('#prefix' => '<div class="container-inline">', '#type' => 'markup', '#value' => '<span id="addCc" class="el" tabindex="2" role="link">Thêm Cc</span>');
    $form ['addBcc'] = array('#suffix' => '</div>', '#type' => 'markup', '#value' => '<span id="addBcc" class="el" tabindex="2" role="link">Thêm Bcc</span>', '#suffix' => '</div>');
    $form ['subject'] = array('#prefix' => '<div class="dK">', '#suffix' => '</div>', '#type' => 'textfield', '#title' => 'Chủ đề ', '#default_value' => $subject, '#maxlength' => 1024);
    if (!empty($att)) {
        $form ['att'] = array('#type' => 'checkboxes', '#options' => $att,);
    }
    $form ['attachments'] = array('#type' => 'markup', '#value' => '<div id="attachments"></div>',);
    $form ['addAttach'] = array('#prefix' => '<div class="container-inline">', '#suffix' => '</div>', '#type' => 'markup', '#value' => '<span id="addAttach" class="el" tabindex="2" role="link">Đính kèm </span>');
    $form ['body'] = array('#type' => 'textarea', '#title' => t('Nội dung thư'), '#default_value' => $body, '#rows' => 15);
    $form ['#attributes']['enctype'] = 'multipart/form-data';
    return $form;
}

//  End of _basic_webmail_send_mail_form().
/**
 * Send the message.
 *
 * @param form_id
 * The form ID of the passed form.
 * @param $form_state
 * The form values of the passed form.
 * @author truongnx.ithut@gmail.com
 */
function basic_webmail_send_mail_form_submit($form, &$form_state) {
    require_once 'libs/class.phpmailer.php';
    require_once 'libs/class.smtp.php';
    global $user;
    //check the username and password for connection
    if (!$user->basic_webmail_username || !$user->basic_webmail_passsword) {
        drupal_set_message('Chưa cấu hình tài khoản truy cập máy chủ SMTP', 'error');
        return FALSE;
    }
    //check the server address and server port
    if (variable_get('basic_webmail_smtp_server', '') == '' || variable_get('basic_webmail_smtp_port', '') == '') {
        drupal_set_message('Chưa cấu hình máy chủ SMTP', 'error');
        return FALSE;
    }
    // Get the users e-mail login and password.
    $username = $user->basic_webmail_username;
    $password = $user->basic_webmail_passsword;
    $email_name = variable_get('basic_webmail_email_name', '');
    $server_address = variable_get('basic_webmail_smtp_server', '');
    $server_port = variable_get('basic_webmail_smtp_port', 25);
    //create new mailer
    $mail = new PHPMailer (); // defaults to using php "mail()"	
    $mail->IsSMTP(); // send via SMTP
    $mail->Host = $server_address; // SMTP servers
    $mail->port = $server_port; //SMTP port
    $mail->SMTPAuth = true; // turn on/off SMTP authentication
    $mail->Username = $username;
    $mail->Password = $password;
    $mail->CharSet = "utf-8";
    $mail->XMailer = 'E-GOV-QLVB-2012';
    $mail->SetFrom($username, $email_name);
    $mail->Subject = $form_state ['values'] ['subject'];
    $mail->Body = $form_state ['values'] ['body'];
    //process sent to,cc,bcc
    $to_list = explode(',', $form_state ['values'] ['sendto']);
    if (!empty($to_list)) {
        foreach ($to_list as $item) {
            $email = _basic_webmail_get_email($item);
            if ($email) {
                $mail->AddAddress($email ['email'], $email ['name']);
            }
        }
    }
    $cc_list = explode(',', $form_state ['values'] ['sendcc']);
    if (!empty($cc_list)) {
        foreach ($cc_list as $item) {
            $email = _basic_webmail_get_email($item);
            if ($email) {
                $mail->AddCC($email ['email'], $email ['name']);
            }
        }
    }
    $bcc_list = explode(',', $form_state ['values'] ['sendbcc']);
    if (!empty($bcc_list)) {
        foreach ($bcc_list as $item) {
            $email = _basic_webmail_get_email($item);
            if ($email) {
                $mail->AddBCC($email ['email'], $email ['name']);
            }
        }
    }
    //process attachments file
    if (!empty($_FILES ['attachments'] ['name'])) {
        foreach ($_FILES ['attachments'] ['name'] as $idx => $item) {
            $filepath = $_FILES ['attachments'] ['tmp_name'] [$idx];
            $mime = module_exists('mimedetect') ? mimedetect_mime($item['filepath']) : file_get_mimetype($filepath);
            $mail->AddAttachment($filepath, $item, 'base64', $mime);
        }
    }
    //process forward attach file
    if (!empty($form_state ['values'] ['att'])) {
        $file_dest = variable_get('basic_webmail_attachment_location', file_directory_path() . '/attachments');
        $file_dest .= '/' . $user->name . '/';
        $replyAttach = array();
        foreach ($form_state ['values'] ['att'] as $item) {
            if ($item) {
                $mail->AddAttachment($file_dest . $form_state ['values'] [$item]);
            }
        }
    }//end of prepare data

    switch ($form_state ['clicked_button'] ['#name']) {
        case 'send' :
            if (!$mail->Send()) {
                drupal_set_message("Mailer Error: " . $mail->ErrorInfo, "error");
            } else {
                drupal_set_message("Đã gửi thư thành công");
                $sent_folder = mb_convert_encoding('Hộp thư đi', 'UTF7-IMAP', 'UTF-8');
                $obj = _basic_webmail_open_connect();
                if ($obj) {
                    $message = $mail->GetMIMEHeader() . $mail->GetMIMEBody();
                    $message = str_replace("\n", "\r\n", $message);
                    $obj->append($sent_folder, $message);
                }
                drupal_goto(base_path() . '/basic_webmail');
            }
            break;
        case 'save' :
            $sent_folder = mb_convert_encoding('Thư nháp', 'UTF7-IMAP', 'UTF-8');
            $obj = _basic_webmail_open_connect();
            if ($obj) {
                $mail->SetMessageType();
                $message = $mail->CreateHeader() . $mail->CreateBody();
                $message = str_replace("\n", "\r\n", $message);
                if ($obj->append($sent_folder, $message)) {
                    drupal_set_message("Đã lưu thư vào Thư nháp");
                } else {
                    drupal_set_message("Có lỗi xảy ra chưa lưu được thư");
                }
                drupal_goto(base_path() . '/basic_webmail');
            }
            break;
        default :
            break;
    }
}

//  End of _basic_webmail_send_mail_form_save().
?>
