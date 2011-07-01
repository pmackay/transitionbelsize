<?php if (!empty($mission)): ?>
  <div id="mission" class="og-mission">
  	<?php if($image): ?>
  		<div><?php print $image; ?></div>
  	<?php endif; ?>
  	<div><span>Manager:<span> <?php print $manager; ?></div>
  	<div><span>Members:<span> <?php print $members; ?></div>
  	<?php print $mission; ?>
  </div>
<?php endif; ?>