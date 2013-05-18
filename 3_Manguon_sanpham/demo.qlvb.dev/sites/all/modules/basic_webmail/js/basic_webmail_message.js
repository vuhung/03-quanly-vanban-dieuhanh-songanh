Drupal.behaviors.basic_webmail = function(context) 
{  
  createMailBox();
  $("#edit-reply").bind("click", function() {	  	  
	  var basePath=Drupal.settings.basic_webmail.basePath;
	  var fid=Drupal.settings.basic_webmail.fid;
	  var mid=Drupal.settings.basic_webmail.mid;
	  window.location = basePath+'basic_webmail/sendmail/reply/' + mid +  '/' + fid; 
	  return false;
  });
  $("#edit-forward").bind("click", function() {	  	  
	  var basePath=Drupal.settings.basic_webmail.basePath;
	  var fid=Drupal.settings.basic_webmail.fid;
	  var mid=Drupal.settings.basic_webmail.mid;
	  window.location=basePath+'basic_webmail/sendmail/forward/' + mid +  '/' + fid; 
	  return false;
  });
  $("#edit-delete").bind("click", function() {	
	  pleaseWait('Vui lòng đợi ...');
	  var fid=Drupal.settings.basic_webmail.fid;
	  var mid=Drupal.settings.basic_webmail.mid;
	  var url=Drupal.settings.basic_webmail.basePath + 'basic_webmail/ajax/request';
	  $.ajax({
	      type: 'POST',
	      url: url,			      
	      dataType: 'json', // define the type of data that is going to
							// get back from the server
	      data: { action: "delete", msgid: mid , boxid :fid },
	      success: function(response) {	
	    	  $.unblockUI();
	    	  if(response.error){
	    		  alert(response.message);
	    	  }else{
	    		  window.location = Drupal.settings.basic_webmail.basePath+'basic_webmail/mailbox/' + fid;
	    	  }
	      }, 
	  });
	  return false;
  });
  $("#edit-move-to").bind("click", function() {	
	  pleaseWait('Vui lòng đợi ...');
	  var fid=Drupal.settings.basic_webmail.fid;
	  var mid=Drupal.settings.basic_webmail.mid;
	  var url=Drupal.settings.basic_webmail.basePath + 'basic_webmail/ajax/request';	  
	  $.ajax({
		  type: 'POST',
		  url: url,			      
		  dataType: 'json', // define the type of data that is going to
		  					// get back from the server
		  data: { action: "moveto", msgid: mid , boxid :fid ,targetid:$('#edit-folder-name').val() },
		  success: function(response) {
			  $.unblockUI();
			  if(response.error){
				  alert(response.message);
			  }else{
				  window.location = Drupal.settings.basic_webmail.basePath+'basic_webmail/mailbox/' + fid;
			  }
		  }, 
	  });
	  return false;
  });
  $("#edit-import").bind("click", function() {
	  
	  var fid=Drupal.settings.basic_webmail.fid;
	  var mid=Drupal.settings.basic_webmail.mid;
	  var url=Drupal.settings.basic_webmail.basePath + 'basic_webmail/ajax/request';
	  var loading=Drupal.settings.basic_webmail.imagePath +'/loading87.gif';
	  pleaseWait('<img  src="'+loading+'" height=20 /> Đang tạo văn bản đến, vui lòng đợi...');
	  $.ajax({
		  type: 'POST',
		  url: url,			      
		  dataType: 'json', // define the type of data that is going to
		  		    // get back from the server
		  data: { action: "import", msgid: mid , boxid :fid },
		  success: function(response) {			  
			  if(response.error){
				  alert(response.message);
			  }else{
				  window.location = Drupal.settings.basePath+'node/' + response.result;
			  }
		  }, 
                  error : function(jqXHR, textStatus, errorThrown){
                      console.log(jqXHR);
                      console.log(textStatus);
                      console.log(errorThrown);
                  },
                  complete: function(){
                        $.unblockUI();
                  },
	  }); 
	  return false;
  });
  

}
