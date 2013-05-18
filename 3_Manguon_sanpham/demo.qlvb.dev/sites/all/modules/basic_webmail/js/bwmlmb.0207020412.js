function createMailBox(){
	d = new dTree('d',Drupal.settings.basic_webmail.imagePath);		
    var mailbox=Drupal.settings.basic_webmail.mailbox;
    var mailCounter=Drupal.settings.basic_webmail.mailCounter
    var i=0;    
	for(i=0; i < mailbox.length ;i++ ){	
		var boxname=mailbox[i].alias;
		if( mailCounter && mailCounter[mailbox[i].id]){
			boxname+=' ('+ mailCounter[mailbox[i].id] +')';
		}else{
			boxname= '<span style="font-weight:normal;">' + boxname + '</span>';
		}
		d.add(mailbox[i].id,mailbox[i].parent,boxname,mailbox[i].link);
	}
	$('#bwmfdl').html(d.toString());
}



