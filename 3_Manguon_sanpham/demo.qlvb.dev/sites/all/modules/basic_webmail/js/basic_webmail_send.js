function removeAttach(child){
	  $(child).parent().remove();
 }
Drupal.behaviors.basic_webmail = function(context) 
{  
  createMailBox();
  var numAttachment=0;
  $('#edit-sendcc-wrapper').hide();
  $('#edit-sendbcc-wrapper').hide();
  $("#addCc").bind("click", function() {	  
	  $('#edit-sendcc-wrapper').show();
	  $(this).hide();
  });
  $("#addBcc").bind("click", function() {
	  $('#edit-sendbcc-wrapper').show();
	  $(this).hide();
  });
  $("#addAttach").bind("click", function() {
	  numAttachment++;
	  name="attachments["+numAttachment+"]";
	  $('#attachments').append('<div><input type="file" name="'+name+'"  class="form-file" id="'+name+'" size="60"><span  class="el"  role="link" onclick="removeAttach(this)"> Xóa </span>'); 
  });
  $("#edit-cancel").bind("click", function() {
	  window.location = Drupal.settings.basePath+'basic_webmail'; 
	  return false;
  });
  $("#edit-send").bind("click", function() {
	 if(!jQuery.trim($('#edit-sendto').val())){
		 $('#edit-sendto').addClass("form-text form-autocomplete required error");
		 return false;
	 }
	 return true;
	  
  });
  
}
