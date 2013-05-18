Drupal.behaviors.basic_webmail = function(context) {
  
    createMailBox();
    //Set style for new email
    markNewEmail();
    //End of set style
	
    //auto refresh
    setInterval(function() {
        window.location.reload( true );
    }, 1000*60*5);
    //end of auto refresh
    $('#edit-mark-unread', context).bind('click', function() {
        var unread=new Array();		
        $('.basic_webmail_mail_list tbody :input:checked').each(function(index) {
            if($(this).parents('tr:first').find('span').html()==''){
                $(this).parents('tr:first').find('span').html('mới');			
                unread.push($(this).parents('tr:first').find('input').val());
            }
						
        });	

        if(unread.length > 0){
            pleaseWait('Vui lòng đợi ...');
            var url=Drupal.settings.basic_webmail.basePath + 'basic_webmail/ajax/request';
            var boxid=Drupal.settings.basic_webmail.boxid;
            var msgid=unread.join(',');
            $.ajax({
                type: 'POST',
                url: url,			      
                dataType: 'json', // define the type of data that is going to
                // get back from the server
                data: {
                    action: "unread", 
                    msgid: msgid ,
                    boxid : boxid
                },
                success: function(response) {	
                    $.unblockUI();
                    if(response.error){
                        alert(response.message);
                    }else{
                        var mailCounter=Drupal.settings.basic_webmail.mailCounter;
                        mailCounter[boxid]=parseInt(mailCounter[boxid]) + parseInt(unread.length);
                    }
                    createMailBox();
                    markNewEmail();
                }, 
            });		
        }
        return false;
    });
    $('#edit-mark-read', context).bind('click', function() {
        var read=new Array();
        $('.basic_webmail_mail_list tbody :input:checked').each(function(index) {			
            if($(this).parents('tr:first').find('span').html()=='mới'){
                $(this).parents('tr:first').find('span').html('');
                read.push($(this).parents('tr:first').find('input').val());
            };
			
        });
        if(read.length > 0){
            pleaseWait('Vui lòng đợi ...');
            var url=Drupal.settings.basic_webmail.basePath + 'basic_webmail/ajax/request';
            var boxid=Drupal.settings.basic_webmail.boxid;
            var msgid=read.join(',');
            $.ajax({
                type: 'POST',
                url: url,			      
                dataType: 'json', // define the type of data that is going to
                // get back from the server
                data: {
                    action: "read", 
                    msgid: msgid , 
                    boxid :boxid
                },
                success: function(response) {
                    $.unblockUI();
                    if(response.error){
                        alert(response.message);
                    }else{
                        var mailCounter=Drupal.settings.basic_webmail.mailCounter;
                        if(mailCounter[boxid] > read.length  ){
                            mailCounter[boxid]=parseInt(mailCounter[boxid]) - parseInt(read.length);
                        }else
                        {
                            mailCounter[boxid]=0
                        }
                        createMailBox();
                        markNewEmail();
			    		
                    }
                }, 
            });	
        }
        return false;
    });
    $('#edit-delete', context).bind('click', function() {
        var del=new Array();
        $('.basic_webmail_mail_list tbody :input:checked').each(function(index) {			
            del.push($(this).parents('tr:first').find('input').val());
        });
        if(del.length > 0){
            pleaseWait('Vui lòng đợi ...');
            var url=Drupal.settings.basic_webmail.basePath + 'basic_webmail/ajax/request';
            var msgid=del.join(',');
            $.ajax({
                type: 'POST',
                url: url,			      
                dataType: 'json', // define the type of data that is going to
                // get back from the server
                data: {
                    action: "delete", 
                    msgid: msgid , 
                    boxid :Drupal.settings.basic_webmail.boxid
                    },
                success: function(response) {	
                    $.unblockUI();
                    if(response.error){
                        alert(response.message);
                    }else{
			    		 
                        window.location.reload( true );
                    }
                }, 
            });	
        }
        return false;
    });
    $('#edit-move-to', context).bind('click', function() {
        var moveto=new Array();
        $('.basic_webmail_mail_list tbody :input:checked').each(function(index) {			
            moveto.push($(this).parents('tr:first').find('input').val());
        });
        if(moveto.length > 0){
            pleaseWait('Vui lòng đợi ...');
            var url=Drupal.settings.basic_webmail.basePath + 'basic_webmail/ajax/request';
            var msgid=moveto.join(',');
            $.ajax({
                type: 'POST',
                url: url,			      
                dataType: 'json', // define the type of data that is going to
                // get back from the server
                data: {
                    action: "moveto", 
                    msgid: msgid , 
                    boxid :Drupal.settings.basic_webmail.boxid ,
                    targetid:$('#edit-folder-name').val()
                    },
                success: function(response) {
                    $.unblockUI();
                    if(response.error){
                        alert(response.message);
                    }else{
                        window.location.reload( true );
                    }
					
                }, 
            });	
        }
        return false;
    });
    $('#edit-refresh', context).bind('click', function() {
        pleaseWait('Đang tải dữ liệu ...');
        window.location.reload( true );
        return false;
    });
}
// Bold subject cua mail moi
function markNewEmail(){
    $('.basic_webmail_mail_list tbody :input').each(function(index) {
        if($(this).parents('tr:first').find('span').html()=='mới'){
            $(this).parents('tr:first').find('td a:first').css('font-weight','bold');
        }else{
            $(this).parents('tr:first').find('td a:first').css('font-weight','normal');
        }     
    });
}