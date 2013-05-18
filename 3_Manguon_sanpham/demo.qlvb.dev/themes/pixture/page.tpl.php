<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" lang="<?php print $language->language ?>" xml:lang="<?php print $language->language ?>">

<head>
  <title><?php print $head_title ?></title>
  <?php print $head ?>
  <?php print $styles ?>
  <?php print $scripts ?>
    <!--[if lt IE 7]>
    <style type="text/css" media="all">@import "<?php print base_path() . path_to_theme() ?>/fix-ie.css";</style>
    <![endif]-->
    <script type="text/javascript"> $(document).ready(function() { 	
	$('#edit-field-003-sokyhieu-0-value').keyup(function() { $(this).val($(this).val().toUpperCase()); }); }); 
        $('#edit-field-004-kyhieuvbdi-0-value').keyup(function() { $(this).val($(this).val().toUpperCase()); });
    </script>    
  <script type="text/javascript">
	<?php /* Needed to avoid Flash of Unstyle Content in IE */ ?>
	
</script>
</head>

<?php
$pixture_width = theme_get_setting('pixture_width');
$pixture_width = pixture_validate_page_width($pixture_width);

?>
<body>
<div id="header">
	<div id="logo">
		<img src="/themes/pixture/header.png" />
		<?php if ($logo) { ?><a href="<?php print $base_path ?>" title="<?php print t('Home') ?>">
		<img src="<?php print $logo ?>" width="1200px" alt="<?php print t('Home') ?>" /></a><?php } ?>
	</div>
	<div id="page-navigation">
		<table>
			<tr>
				<td><div id="menu_menutop"><?php print $header ?></div></td> 					
				<td>
					<div id="user_profile">
						<?php if($user->uid) { ?> 
							<ul>
							<li class="name"><?php print l($user->name,'user')?></li>							
							<li class="date"><?php print sprintf("%s ngày %s tháng %s năm %s",t(date("D")),date('d'),date('m'),date('Y')); ?></li>
							<li class="logout"><?php print l(t('logout'),'logout') ?></li>
							</ul>
						<?php } ?>
					</div>
				</td>
			</tr>
		</table>
		
	</div>
	
</div>


<div id="wrapper" style="width: <?php print $pixture_width; ?>;">

<div id="container">

<table border="0" cellpadding="0" cellspacing="0" id="content">
  <tr>

  <?php if ($left) { ?>
  <td id="sidebar-left" class="sidebar">
    <?php print $left ?>
  </td>
  <?php } ?>

  <td valign="top" style="text-align: left;"> 
  <?php if ($mission) { ?><div id="mission"><?php print $mission ?></div><?php } ?>
    <div id="main">
    <?php print $breadcrumb ?>
    <?php if(!empty($node) && $page == 0) { ?>
      <div id="cr8"></div>
    <?php } else { ?>
      <?php if($title) { ?>
            <h1 class="pagetitle"><?php print $title ?></h1>
      <?php } ?>
    <?php } ?>
    <div class="tabs"><?php print $tabs ?></div>
    <?php print $help ?>
    <?php print $messages ?>
	<div id="content-content">
		<?php print $content; ?>
	</div>
    <?php print $feed_icons; ?>
    </div>
  </td>

  <?php if ($right) { ?>
  <td id="sidebar-right" class="sidebar">
    <?php print $right ?>
  </td>
  <?php } ?>

  </tr>
</table>

</div><!-- end of div#container -->

<div id="footer"><?php print $footer_message . $footer ?></div><?php print $closure ?>


</div><!-- end of div#wrapper -->

</body>
</html>
