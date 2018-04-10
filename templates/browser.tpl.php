<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" lang="<?php print $GLOBALS['language']->language; ?>" xml:lang="<?php print $GLOBALS['language']->language; ?>">

<head>
	<title><?php print t('Library Browser');?></title>
	<meta name="robots" content="noindex,nofollow" />
	<?php print drupal_get_html_head(); ?>
	<?php print drupal_get_css(); ?>
	<?php print drupal_get_js('header');?>
	<style media="all" type="text/css">/*Quick-override*/</style>
</head>

<body>
<div id="browser-title"><h1><?php print t('Library Browser');?></h1></div>

<?php print $content;?>
<?php print drupal_get_js('footer'); ?>
</body>

</html>