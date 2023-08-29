<?php include "includes/header.php" ?>

<?php $images=glob("img/gallery/*"); ?>

<section class="gallery">
	<aside class="image-wrapper">
		<img src="<?= $images[0] ?>" alt="">
	</aside>
	<div class="images">

		<?php foreach ($images as $image): ?>
			<a href="<?= $image ?>">
				<img src="<?= $image ?>" alt="">
			</a>
		<?php endforeach ?>

	</div>
</section>

<?php include "includes/footer.php" ?>