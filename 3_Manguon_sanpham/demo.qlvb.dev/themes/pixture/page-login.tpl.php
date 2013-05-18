<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" lang="<?php print $language->language ?>" xml:lang="<?php print $language->language ?>">

<head>
  <title><?php print $head_title ?></title> 
  <link href="/themes/pixture/login_style.css?y" media="all" rel="stylesheet" type="text/css">
    <!--[if lt IE 7]>
    <style type="text/css" media="all">@import "<?php print base_path() . path_to_theme() ?>/fix-ie.css";</style>
    <![endif]-->    
  <script type="text/javascript">
	<?php /* Needed to avoid Flash of Unstyle Content in IE */ ?>	
  </script>
</head>
<body class="box">
<div  id="container">		
    <div id="header"> 
         <img src="/themes/pixture/logo_qlvb.png"></img>        
    </div> 
    
    <div id="content">        
        <?php print $messages ?>
	<div id="content-content">
	<?php print $content; ?>
	</div>
        <div class="reset-password"><a href="/user/password">Yêu cầu mật khẩu mới?</a></div>
    </div>
    <div id="footer-container">
            <div  id="footer">
                <img src="/themes/pixture/drupal_logo.gif" style="float:left;"/> 
                <p class="version-info">PHẦN MỀM QUẢN LÝ VĂN BẢN VÀ ĐIỀU HÀNH CÔNG VIỆC ( version 1.3 )</p>
                <p> &copy; 2012 Trung tâm CNTT - Sở Thông tin và Truyền thông Tỉnh Bắc Giang </p>
                <p> ĐT: 0240.6 280 280 </p>
                <p> Fax: 0240. 3 555 992</p>                                   
            </div>
     </div>
</div>

</body>
</body>
</html>
