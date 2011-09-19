<?php if (!empty($mission)): ?>
  <div id="mission" class="og-mission">
  	<?php if($image): ?>
  		<div style="float: right; padding: 5px"><?php print $image; ?></div>
  	<?php endif; ?>
  	<div style="font-size: 1.3em;"><span>Manager:<span> <?php print $manager; ?></div>
  	<div style="font-size: 1.3em;"><span>Members:<span> <?php print $members; ?></div>
  	<?php print $mission; ?>
  </div>
<?php endif; ?>