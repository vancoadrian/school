<?php 
	require "functions.php";

	if (isset($_GET["lang"])) $lang = $_GET["lang"];
	else $lang = "sk";

	$active_page_path = $_SERVER["PHP_SELF"];
	$active_page_path = explode('/', $active_page_path);
	$active_page = array_pop($active_page_path);

	$page = getpage($active_page, $lang);
 ?>


<!DOCTYPE html>
<html lang="<?= $lang ?>">
<head>
	<meta charset="UTF-8">
	<title><?= $page->maintitle ?></title>
	<link rel="stylesheet" href="css/style.css">
	<link rel="shortcut icon" type="image/png" href="img/logo/favicon.png">
</head>
<body>
<header>
	<img src="img/logo/logo<?= $lang ?>.svg" width="200" class="logo" alt="">
	<img src="img/logo/horor.svg" width="350" class="title" alt="">

<nav class="language">
	<ul>
		<li>
			<a href="?lang=sk"><img src="img/logo/svk.svg" alt="SVK" width="100"></a>
		</li>
		<li>
			<a href="?lang=en"><img src="img/logo/uk.svg" alt="EN" width="100"></a>
		</li>
	</ul>
</nav>

</header>
<nav class="navigation">
	<ul>
		<?php foreach (getpages() as $this_page): ?>
			<li><a href="<?= $this_page->page ?>?lang=<?= $lang ?>"><?= $this_page->{"nadpis_".$lang} ?></a></li>
		<?php endforeach ?>
	</ul>
</nav>