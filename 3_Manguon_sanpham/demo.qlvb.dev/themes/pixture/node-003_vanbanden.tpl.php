<?php

/**
 * @file node.tpl.php
 *
 * Theme implementation to display a node.
 *
 * Available variables:
 * - $title: the (sanitized) title of the node.
 * - $content: Node body or teaser depending on $teaser flag.
 * - $picture: The authors picture of the node output from
 *   theme_user_picture().
 * - $date: Formatted creation date (use $created to reformat with
 *   format_date()).
 * - $links: Themed links like "Read more", "Add new comment", etc. output
 *   from theme_links().
 * - $name: Themed username of node author output from theme_username().
 * - $node_url: Direct url of the current node.
 * - $terms: the themed list of taxonomy term links output from theme_links().
 * - $submitted: themed submission information output from
 *   theme_node_submitted().
 *
 * Other variables:
 * - $node: Full node object. Contains data that may not be safe.
 * - $type: Node type, i.e. story, page, blog, etc.
 * - $comment_count: Number of comments attached to the node.
 * - $uid: User ID of the node author.
 * - $created: Time the node was published formatted in Unix timestamp.
 * - $zebra: Outputs either "even" or "odd". Useful for zebra striping in
 *   teaser listings.
 * - $id: Position of the node. Increments each time it's output.
 *
 * Node status variables:
 * - $teaser: Flag for the teaser state.
 * - $page: Flag for the full page state.
 * - $promote: Flag for front page promotion state.
 * - $sticky: Flags for sticky post setting.
 * - $status: Flag for published status.
 * - $comment: State of comment settings for the node.
 * - $readmore: Flags true if the teaser content of the node cannot hold the
 *   main body content.
 * - $is_front: Flags true when presented in the front page.
 * - $logged_in: Flags true when the current user is a logged-in member.
 * - $is_admin: Flags true when the current user is an administrator.
 *
 * @see template_preprocess()
 * @see template_preprocess_node()
 */
 ?>
<table class="node<?php if ($teaser) { if($sticky) { print " sticky"; } else { print " teaser"; } } ?><?php if (!$status) print " node-unpublished"; ?>"> 
  <?php 
    if($teaser) {
      if($sticky) $prefix = 'sticky-'; 
      else $prefix = 'teaser-';
    }
    else {
      $prefix = '';
    }
  ?>



  <tr>

  <td class="<?php print $prefix; ?>node-mc">

    <table>
    <tr>
      <td style="border:none;">	
		<?php if ($links) { ?><div class="popups-links"><?php print $links?></div><?php }; ?>
        <h2 class="title"><a href="<?php print $node_url?>"  style="text-decoration:none;"><?php print $title?></a></h2>
        <!---
        <span class="submitted"><?php //print $submitted?></span>
        <span class="taxonomy"><?php //print $terms?></span> 
        --->			       
        		
      </td>
    </tr>
    </table>
    <?php if($picture) print $picture; ?>
    <div class="content"><?php print $content?></div>	
    <?php if ($links) { ?><div class="links"><?php print $links?></div><?php }; ?>
	
  </td>

  </tr>

  <tr>

  <tr>
</table>

