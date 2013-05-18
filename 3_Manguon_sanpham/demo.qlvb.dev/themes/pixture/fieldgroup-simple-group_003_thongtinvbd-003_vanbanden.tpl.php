<?php
// $Id: fieldgroup-simple.tpl.php,v 1.1.2.1 2009/02/28 23:56:17 yched Exp $

/**
 * @file fieldgroup-simple.tpl.php
 * Default theme implementation to display the a 'simple-styled' fieldgroup.
 *
 * Available variables:
 * - $group - The group field content
 * - $group_name - The group name
 * - $group_name_css - The css-compatible group name.
 * - $label - The group label
 * - $description - The group description
 * - $content - The group content 
 *
 * @see template_preprocess_fieldgroup_simple()
 */
?>
<?php if ($content) : ?>
    <fieldset class="fieldgroup <?php print $group_name_css; ?> collapsible">
		<legend><?php if ($label): print $label; endif; ?> </legend>
			
	    <div class="fieldset-wrapper">		
			<table >		  	  
				<tbody>			  
					<tr>				  
						<td><?php print $group['field_003_ngayvbden'] ?></td>
						<td><?php print $group['field_003_sovbden'] ?></td>
						<td><?php print $group['field_003_sokyhieu'] ?></td>
					</tr>
					<tr>
						<td colspan="2"><?php print $group['field_003_dvguivb_text'] ?></td>
						<td><?php print $group['field_003_loaivbd'] ?></td>		  
					</tr>			 			 
					<tr>
						<td td colspan="3"><?php print $group['field_003_fileganvbd'] ?></td>				  				  		
				  </tr>			 			 
				</tbody>
			</table>
		</div>
		
	</fieldset>
<?php endif;?>











</fieldset>