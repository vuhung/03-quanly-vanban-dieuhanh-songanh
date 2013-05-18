<?php
function basic_webmail_mail_list($path = 'mailbox', $boxid = 1) {
    basic_webmail_add_js();
    $mailbox = basic_webmail_get_mailbox ();
    if (! $mailbox) {
            drupal_set_message ( 'Error: Can not get mailbox from IMAP Server', 'error' );
            return '';
    }
    if(!isset ( $mailbox [$boxid])){
      $boxid=1;
    }
    // Current mailbox.
    $box = $mailbox [$boxid];

    // Check new email for all mailbox.
    basic_webmail_get_new_email ( $box );
    $mailCounter=_basic_webmail_count_new( $mailbox );
    //count total email
    $total_email=_basic_webmail_count_total($box['id'] );
    if($total_email){
            drupal_set_title(t('@mailbox (@total email)',array('@mailbox'=>$box['alias'],'@total'=>$total_email)));
    }else{
            drupal_set_title(t('@mailbox ',array('@mailbox'=>$box['alias'])));
    }
    //add js
    $setting = array (
             'basePath' => base_path (),
             'mailbox' => array_values ( $mailbox ),
             'boxid' => $box ['id'],
             'mailCounter' => $mailCounter,
           );
    drupal_add_js ( array ('basic_webmail' => $setting ), 'setting' );
    drupal_add_js ( drupal_get_path ( 'module', 'basic_webmail' ) . '/js/basic_webmail_list.js' );
    //end of add js
    $output = '';
    $items = _basic_webmail_check_internal_email ();
    $counter = count ( $items );
    if ($counter) {
            $output .= drupal_get_form ( "basic_webmail_mail_notify_form", $counter );
    }
    // Set the title of the page to the current mailbox, if there is one.
    $output .= '<div class="basic_webmail_main">';
    $output .= '<div class="basic_webmail_navigate">';
    $output .= '<div class="basic_mail_compose_mail">' . l ( 'Soạn', 'basic_webmail/sendmail' ) . '</div>';
    $output .= '<div class="basic_mail_folder_list" id="bwmfdl"></div>';
    $output .= '</div>';
    $output .= '<div class="basic_webmail_mail_list">';
    $output .= '<div>';
    $output .= drupal_get_form ( "basic_webmail_mail_list_form", $path, $boxid );
    $output .= '</div>';
    $output .= '<div>' . theme ( 'pager', NULL, variable_get ( 'basic_webmail_messages_per_page', 25 ), 0 ) . '</div>';
    $output .= '</div>';
    $output .= '</div>';
    return $output;
}

/**
 * Enter description here ...
 * @param mailbox_info $box
 * @author truongnx.ithut@gmail.com
 */
function basic_webmail_get_new_email($box) {
  
	Global $user;
    $last_uid = variable_get ( 'basic_webmail_last_uid_' . $user->uid, array () ); 
    $num_per_page = variable_get ( 'basic_webmail_messages_per_page', 25 );
    $folder = mb_convert_encoding ( $box ['fullname'], 'UTF7-IMAP', 'UTF-8' );
    $num = variable_get ( 'basic_webmail_subject_characters', 40 );
    $obj=_basic_webmail_open_connect($folder);
    if(!$obj){
        return FALSE;
    }    
	$box_uid = $last_uid [$box ['id']];	
	//check whether reload or not
    $status = $obj->get_status ( $folder );	
    if ($status->messages > $num_per_page && isset ( $box_uid ) && $box_uid ['uidvalidity'] == $status->uidvalidity) {
      $reload = FALSE;           
    }else {
      $reload= TRUE;
    }    
    //end of check  
    if ($reload) {
        if (! db_query ( 'DELETE FROM {basic_webmail_messages} WHERE user_id = %d AND mboxid = %d', $user->uid, $box ['id'] )) {
            watchdog ( 'basic_webmail', 'There was an error deleting old message records from the basic_webmail_messages table.', array (), WATCHDOG_ERROR );
        }
        $list_mail = $obj->get_overview ();
    } else {            
        $list_mail = $obj->get_overview_uid ( $box_uid ['uid'] ); 
    }
    // Nếu không có mail 
    if(!$list_mail){      
      return 0;
    }
    // Nếu không reload thì bỏ email đầu tiên
    if(!$reload){
      array_shift ( $list_mail );
    }
    // If there aren't new emails
    if(empty($list_mail)){
      return 0;
    }
    // Update new email
    // Update last uid
    $lastest_uid = end ( $list_mail )->uid;
    $last_uid [$box ['id']] = array ('uid' => $lastest_uid, 'uidvalidity' => $status->uidvalidity );
    variable_set ( 'basic_webmail_last_uid_' . $user->uid, $last_uid ); 
    foreach ( $list_mail as $item ) {
        $subject = _basic_webmail_decode_mime_str ( $item->subject );
        if (! $subject) {
            $subject = '(Không chủ đề)';
        }
        $subject = truncate_utf8 ( $subject, $num, TRUE, TRUE );
        //from
        $from = $item->from;
        if (! $from) {
            $from = '(Chưa biết)';
        } else {
            if (preg_match ( '/\s*(.*)\s*(<.*>)/', $from, $match )) {
                $personal = _basic_webmail_decode_mime_str ( $match [1] );
                $from = l ( $personal, 'basic_webmail/sendmail/' . $personal . $match [2] );
            } else {
                $from = l ( $from, 'basic_webmail/sendmail/' . $from );
            }
        }
        $tmp= explode(' ', trim($item->date));
        $time=_basic_webmail_get_timestamp($tmp);
        //date
        if (! db_query ( 'INSERT INTO {basic_webmail_messages} (user_id, mid, msubject, mfrom, mdate, mread, manswer, mboxid) 
                        VALUES (%d, %d, "%s", "%s", "%s", %d, %d, %d)', $user->uid, $item->uid, $subject, $from, $time, $item->seen, $item->answered, $box ['id'] )) {
            watchdog ( 'basic_webmail', 'There was an error inserting message records into the basic_webmail_messages table.', array (), WATCHDOG_ERROR );
        }
    }
}
function basic_webmail_mail_notify_form(&$form_state, $counter) {
	$notify = t ( "Hiện tại đang có @counter email đến từ hệ thống", array ('@counter' => $counter ) );
	$notify .= l ( ' Click vào đây', 'basic_webmail/batch_import' );
	$notify .= t ( " để tạo thành văn bản đến" );
	$form ['info'] = array ('#prefix' => '<div class="messages warning">', '#suffix' => '</div>', '#type' => 'markup', '#value' => $notify );
	return $form;
}
function basic_webmail_mail_list_form(&$form_state, $path = 'mailbox', $boxid = 1) {
	Global $user;
	// Set the path to the icons.
	$mailbox = basic_webmail_get_mailbox ();
	$icon_path = drupal_get_path ( 'module', 'basic_webmail' ) . '/icons/';
	$header = array ( 
					 array ('data' => t ( 'Chưa đọc' ), 'field' => 'mread' ), 
	                 array ('data' => t ( 'Trả lời' ), 'field' => 'manswer' ), 
	                 array ('data' => t ( 'Chủ đề' ), 'field' => 'msubject' ), 
	                 array ('data' => t ( 'Người gửi' ), 'field' => 'mfrom' ), 
	                 array ('data' => t ( 'Thời gian' ), 'field' => 'mdate', 'sort' => 'desc' ) );
	$query = 'SELECT * FROM {basic_webmail_messages} WHERE user_id = %d AND mboxid = %d';
	$tablesort = tablesort_sql ( $header );
	$rows = array ();
	$result = pager_query ( $query . $tablesort, variable_get ( 'basic_webmail_messages_per_page', 25 ), 0, NULL, $user->uid, $boxid );
	while ( $message = db_fetch_object ( $result ) ) {
		$messages [$message->mid] = '';
		$form ['mread'] [$message->mid] = array ('#value' => $message->mread ? '<span class="marker"></span>' : '<span class="marker">mới</span>', '#prefix' => '<div class="column-center">', '#suffix' => '</div>' );
		$form ['manswer'] [$message->mid] = array ('#value' => $message->manswer ? theme_image ( $icon_path . 'replied.png' ) : '', '#prefix' => '<div class="column-center">', '#suffix' => '</div>' );
		$mail_link = l ( $message->msubject, 'basic_webmail/message/' . $message->mid . '/' . $message->mboxid );
		$form ['msubject'] [$message->mid] = array ('#value' =>  $mail_link  );
		$form ['mfrom'] [$message->mid] = array ('#value' => $message->mfrom );
		if ($message->mdate == 0) {
			$form ['mdate'] [$message->mid] = array ('#value' => '(Invalid date.)' );
		} else {
			
			$date = date ( 'dmY', $message->mdate );
			if ($date == date ( 'dmY' )) {
				$day_name = sprintf("Hôm nay,%s",date ( "H\hi'", $message->mdate));
			} else {
				$day_name = sprintf("%s,%s",date ( "d/m/Y", $message->mdate),date("H\\hi'", $message->mdate ));
			}
			$form ['mdate'] [$message->mid] = array ('#value' => $day_name );
		}
	}
	$form ['messages'] = array ('#type' => 'checkboxes', '#options' => $messages );
	$form ['refresh'] = array ('#type' => 'submit', '#value' => t ( 'Tải lại' ), '#prefix' => '<div class="container-inline">' );
	$form ['mark_unread'] = array ('#type' => 'button', '#value' => t ( 'Đánh dấu mới' ) );
	$form ['mark_read'] = array ('#type' => 'button', '#value' => t ( 'Đánh dấu đọc' ) );
	$form ['delete'] = array ('#type' => 'submit', '#value' => t ( 'Xóa thư' ) );
	$form ['move_to'] = array ('#type' => 'submit', '#value' => t ( 'Thực hiện' ) );
	$form ['move'] = array ('#type' => 'markup', '#value' => ' Chuyển tới' );
	foreach ( $mailbox as $item ) {
		if ($item ['parent'] == - 1 || $item ['id'] == $boxid) {
			continue;
		}
		$option [$item ['id']] = $item ['alias'];
	}
	$form ['folder_name'] = array ('#type' => 'select', '#options' => $option );
	return $form;
} //  End of basic_webmail_mail_list_form().
/**
 * Theme the message listing page.
 * @param form
 * The form to theme.
 * @return
 * The themed listing.
 */

function theme_basic_webmail_mail_list_form($form) {
	// Overview table:  
	$header = array (theme ( 'table_select_header_cell' ),
	                 array ('data' => t ( 'Chưa đọc' ), 'field' => 'mread' ), 
	                 array ('data' => t ( 'Trả lời' ), 'field' => 'manswer' ), 
	                 array ('data' => t ( 'Chủ đề' ), 'field' => 'msubject' ), 
	                 array ('data' => t ( 'Người gửi' ), 'field' => 'mfrom' ), 
	                 array ('data' => t ( 'Thời gian' ), 'field' => 'mdate', 'sort' => 'desc' ) );
	$output .= '<div class="basic_webmail_mail_list_operation">';
	$output .= drupal_render ( $form ['refresh'] );
	$output .= drupal_render ( $form ['mark_unread'] );
	$output .= drupal_render ( $form ['mark_read'] );
	$output .= drupal_render ( $form ['delete'] );
	$output .= drupal_render ( $form ['options'] );
	$output .= drupal_render ( $form ['move'] );
	$output .= drupal_render ( $form ['folder_name'] );
	$output .= drupal_render ( $form ['move_to'] );
	$output .= '</div>';
	if (isset ( $form ['msubject'] ) && is_array ( $form ['msubject'] )) {
		foreach ( element_children ( $form ['msubject'] ) as $key ) {
			$rows [] = array (drupal_render ( $form ['messages'] [$key] ), drupal_render ( $form ['mread'] [$key] ), drupal_render ( $form ['manswer'] [$key] ), drupal_render ( $form ['msubject'] [$key] ), drupal_render ( $form ['mfrom'] [$key] ), drupal_render ( $form ['mdate'] [$key] ) );
		}
	} else {
		$rows [] = array (array ('data' => t ( ' Không có Email' ), 'colspan' => '6' ) );
	}
	$output .= theme ( 'table', $header, $rows );
	$output .= drupal_render ( $form );
	return $output;
} //  End of theme_basic_webmail_mail_list_form().
function _basic_webmail_count_total($fid){
	Global $user;
	$result=db_query("SELECT count(1) total FROM {basic_webmail_messages}
					   WHERE user_id=%d  AND mboxid =%d  GROUP BY mboxid",$user->uid,$fid);
					 
	return db_result($result);
}

?>