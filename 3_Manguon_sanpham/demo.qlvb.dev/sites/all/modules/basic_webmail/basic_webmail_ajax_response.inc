<?php

/**
 * 
 * Response all ajax request 
 * @author truongnx.ithut@gmail.com
 */
function basic_webmail_ajax_response() {
    if (!isset($_POST['action'])) {
        return;
    }
    $action = $_POST['action'];
    switch ($action) {
        case 'unread':
            _basic_webmail_mark_unread();
            break;
        case 'read':
            _basic_webmail_mark_read();
            break;
        case 'delete':
            _basic_webmail_delete();
            break;
        case 'moveto':
            _basic_webmail_moveto();
            break;
        case 'import':
            _basic_webmail_import();
    };
}

/**
 * 
 */
function _basic_webmail_mark_unread() {
    global $user;
    if (!isset($_POST['msgid'])) {
        exit();
    }
    $mailbox = basic_webmail_get_mailbox();
    $boxid = $_POST['boxid'];
    $box = $mailbox[$boxid];
    if (!$box) {
        print drupal_to_js(array('error' => 1, 'message' => 'Không tồn tại hộp thư'));
        exit();
    }
    $folder = mb_convert_encoding($box['fullname'], 'UTF7-IMAP', 'UTF-8');
    $obj = _basic_webmail_open_connect($folder);
    if (!$obj) {
        print drupal_to_js(array('error' => 1, 'message' => 'Không kết nối được tới máy chủ IMAP'));
        exit();
    }
    $msgid = $_POST['msgid'];
    $obj->clear_flag($msgid, "\\SEEN");
    unset($obj);

    //to avoid reload need set_flag to temp table in database
    $mid = explode(',', $msgid);
    $placeHolder = db_placeholders($mid, 'int');
    $mid[] = $user->uid;
    $mid[] = $boxid;
    if (!db_query("UPDATE {basic_webmail_messages} SET  mread=0 WHERE mid IN($placeHolder) AND user_id = %d AND mboxid = %d", $mid)) {
        print drupal_to_js(array('error' => 1, 'message' => 'Có lỗi khi cập nhật vào cơ sở dữ liệu'));
        exit();
    }
    print drupal_to_js(array('error' => 0, 'message' => ''));
    exit();
}

/**
 * 
 */
function _basic_webmail_mark_read() {
    global $user;
    if (!isset($_POST['msgid'])) {
        exit();
    }
    $mailbox = basic_webmail_get_mailbox();
    $boxid = $_POST['boxid'];
    $box = $mailbox[$boxid];
    if (!$box) {
        print drupal_to_js(array('error' => 1, 'message' => 'Không tồn tại hộp thư'));
        exit();
    }
    $folder = mb_convert_encoding($box['fullname'], 'UTF7-IMAP', 'UTF-8');
    $obj = _basic_webmail_open_connect($folder);
    if (!$obj) {
        print drupal_to_js(array('error' => 1, 'message' => 'Không kết nối được tới máy chủ IMAP'));
        exit();
    }
    $msgid = $_POST['msgid'];
    $obj->set_flag($msgid, "\\SEEN");
    unset($obj);
    //to avoid reload need set_flag to temp table in database
    $mid = explode(',', $msgid);
    $placeHolder = db_placeholders($mid, 'int');
    $mid[] = $user->uid;
    $mid[] = $boxid;
    if (!db_query("UPDATE {basic_webmail_messages} SET  mread=1 WHERE  mid IN($placeHolder) AND user_id = %d AND mboxid = %d", $mid)) {
        print drupal_to_js(array('error' => 1, 'message' => 'Có lỗi khi cập nhật vào cơ sở dữ liệu'));
        exit();
    }
    print drupal_to_js(array('error' => 0, 'message' => ''));
    exit();
}

/**
 * 
 * Enter description here ...
 * @author truongnx.ithut@gmail.com
 */
function _basic_webmail_delete() {
    global $user;
    if (!isset($_POST['msgid'])) {
        return;
    }
    $mailbox = basic_webmail_get_mailbox();
    $boxid = $_POST['boxid'];
    $box = $mailbox[$boxid];
    if (!$box) {
        print drupal_to_js(array('error' => 1, 'message' => 'Không tồn tại hộp thư'));
        exit();
    }
    $folder = mb_convert_encoding($box['fullname'], 'UTF7-IMAP', 'UTF-8');
    $obj = _basic_webmail_open_connect($folder);
    if (!$obj) {
        print drupal_to_js(array('error' => 1, 'message' => 'Không kết nối được tới máy chủ IMAP'));
        exit();
    }
    $msgid = $_POST['msgid'];
    $mid = explode(',', $msgid);
    $placeHolder = db_placeholders($mid, 'int');
    $mid[] = $user->uid;
    $mid[] = $boxid;
    if ($boxid == 4) {
        if ($obj->delete($msgid)) {
            if (!db_query("DELETE FROM {basic_webmail_messages} WHERE mid IN($placeHolder) AND user_id = %d AND mboxid = %d ", $mid)) {
                print drupal_to_js(array('error' => 1, 'message' => 'Có lỗi khi cập nhật vào cơ sở dữ liệu'));
                exit();
            }
        } else {
            print drupal_to_js(array('error' => 1, 'message' => 'Xóa thư không thành công'));
            exit();
        }
    } else {
        $destination = mb_convert_encoding($mailbox[4]['fullname'], 'UTF7-IMAP', 'UTF-8');
        if (!$obj->get_status($destination)) {
            if (!$obj->add_folder($destination)) {
                print drupal_to_js(array('error' => 1, 'message' => 'Hộp thư Thùng rác không tồn tại, tuy nhiên hệ thống cố gắng tạo hộp thư này trên máy chủ IMAP không thành công'));
                exit();
            }
        }
        if ($obj->move($msgid, $destination)) {
            if (!db_query("DELETE FROM {basic_webmail_messages} WHERE mid IN($placeHolder) AND user_id = %d AND mboxid = %d ", $mid)) {
                print drupal_to_js(array('error' => 1, 'message' => 'Có lỗi khi cập nhật vào cơ sở dữ liệu'));
                exit();
            }
        } else {
            print drupal_to_js(array('error' => 1, 'message' => 'Xóa thư không thành công'));
            exit();
        }
    }
    $obj->expunge();
    unset($obj);
    print drupal_to_js(array('error' => 0, 'message' => ''));
    exit();
}

function _basic_webmail_moveto() {
    global $user;
    if (!isset($_POST['msgid'])) {
        return;
    }
    $mailbox = basic_webmail_get_mailbox();
    $boxid = $_POST['boxid'];
    $targetid = $_POST['targetid'];
    $box = $mailbox[$boxid];
    $target = $mailbox[$targetid];
    if (!$box || !$target) {
        print drupal_to_js(array('error' => 1, 'message' => 'Không tồn tại hộp thư nguồn hoặc hộp thư đích'));
        exit();
    }
    $folder = mb_convert_encoding($box['fullname'], 'UTF7-IMAP', 'UTF-8');
    $obj = _basic_webmail_open_connect($folder);
    if (!$obj) {
        print drupal_to_js(array('error' => 1, 'message' => 'Không kết nối được tới máy chủ IMAP'));
        exit();
    }
    //get message id
    $msgid = $_POST['msgid'];
    $mid = explode(',', $msgid);
    $placeHolder = db_placeholders($mid, 'int');
    $mid[] = $user->uid;
    $mid[] = $boxid;
    $mid = array_merge(array($targetid), $mid);
    //check destination folder
    $destination = mb_convert_encoding($target['fullname'], 'UTF7-IMAP', 'UTF-8');
    if (!$obj->get_status($destination)) {
        if (!$obj->add_folder($destination)) {
            print drupal_to_js(array('error' => 1, 'message' => 'Hộp thư đích không tồn tại, tuy nhiên hệ thống cố gắng tạo hộp thư này trên máy chủ IMAP không thành công'));
            exit();
        }
    }
    if ($obj->move($msgid, $destination)) {
        if (!db_query("DELETE FROM {basic_webmail_messages} WHERE mid IN($placeHolder) AND user_id = %d AND mboxid = %d ", $mid)) {
            print drupal_to_js(array('error' => 1, 'message' => 'Có lỗi khi cập nhật vào cơ sở dữ liệu'));
            exit();
        }
    } else {
        print drupal_to_js(array('error' => 1, 'message' => 'Di chuyển thư không thành công'));
        exit();
    }

    $obj->expunge();
    unset($obj);
    print drupal_to_js(array('error' => 0, 'message' => ''));
    exit();
}

function _basic_webmail_import() {
    global $user;
    $mid = intval($_POST['msgid']);
    $fid = intval($_POST['boxid']);
    if (!$mid || !$fid) {
        print drupal_to_js(array('error' => 1, 'message' => 'Email hoặc Hộp thư không hợp lệ'));
        exit();
    }
    //create connection
    $mailbox = basic_webmail_get_mailbox();
    if (!isset($mailbox[$fid])) {
        print drupal_to_js(array('error' => 1, 'message' => 'Hộp thư không tồn tại'));
        exit();
    }
    $box = $mailbox[$fid];
    $folder = mb_convert_encoding($box['fullname'], 'UTF7-IMAP', 'UTF-8');
    $obj = _basic_webmail_open_connect($folder);
    if (!$obj) {
        print drupal_to_js(array('error' => 1, 'message' => 'Không kết nối được tới máy chủ IMAP'));
        exit();
    }
    //get email
    $message = basic_webmail_get_one_message($mid, $fid, $obj);
    if (!$message) {
        print drupal_to_js(array('error' => 1, 'message' => 'Có lỗi xảy ra khi lấy mail từ IMAP server'));
        exit();
    }
    $email = $message['fromaddress']->mailbox . '@' . $message['fromaddress']->host;
    $name = _basic_webmail_decode_mime_str($message['fromaddress']->personal);
    $node = _basic_webmail_in_system($message['html'][0], $email);
    if (!$node) {
        $node = array('title' => $message['subject'],
            'field_003_loaivbd' => 100,
            'field_003_sokyhieu' => NULL,
            'field_003_nam' => date('Y'));
    }
    $node['email'] = $email;
    $node['name'] = $name;
    $nid = basic_webmail_create_vbd($node, $message['attachments']);
    if (!$nid) {
        print drupal_to_js(array('error' => 0, 'message' => 'Có lỗi xẩy ra trong quá trình tạo văn bản'));
        exit();
    }
    //xóa email sau khi import
    $action_after_import = variable_get("basic_webmail_delete_after_import", 1);
    // Nếu xóa Email
    if ($action_after_import != 1) {
        $obj->delete($mid);
        // Nếu chỉ xóa vào thùng rác
        if ($action_after_import == 2) {
            $destination = mb_convert_encoding($mailbox[4]['fullname'], 'UTF7-IMAP', 'UTF-8');
            if (!$obj->get_status($destination)) {
                if (!$obj->add_folder($destination)) {
                    print drupal_to_js(array('error' => 1, 'message' => 'Hộp thư Thùng rác không tồn tại, tuy nhiên hệ thống cố gắng tạo hộp thư này trên máy chủ IMAP không thành công'));
                    exit();
                }
            }
            if (!$obj->move($mid, $destination)) {
                print drupal_to_js(array('error' => 1, 'message' => 'Xóa thư không thành công'));
                exit();
            }
        }
        // Xóa thư đã được import trong database    
        if (!db_query("DELETE FROM {basic_webmail_messages} WHERE mid=%d AND user_id = %d AND mboxid = %d ", $mid, $user->uid, $fid)) {
            print drupal_to_js(array('error' => 1, 'message' => 'Có lỗi khi xóa mail từ  cơ sở dữ liệu'));
            exit();
        }
        //execute delete mail
        $obj->expunge();
    }

    unset($obj);
    print drupal_to_js(array('error' => 0, 'message' => '', 'result' => $nid));
    exit();
}

?>