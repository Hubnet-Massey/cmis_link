<?php 

/**
 * 
 */
function theme_cmis_link($variables) {
	
	//Note: function currently not called
	$contents = '';
	
	error_log('theme_cmis_link $variables: '.var_export($variables,true));
	
	//add children, breadcrumb helper functions
	$contents .= theme('cmis_link_browse_breadcrumb', array('bcarray' => $variables['bcarray']));
	$contents .= theme('cmis_link_browse_children', array('context' => $variables['children']));
	return $contents;
}


function theme_cmis_link_browse_children($variables) {
	$header = array(t('Name'),t('Type'),t('Size'),t('Author'),t('Last Modified'));
	$rows = array();
	$folder_img = theme('image', array('path' => drupal_get_path('module', 'cmis_link').'/images/space.gif')); //need to add images -placeholder
	$document_img = theme('image', array('path' => drupal_get_path('module', 'cmis_link').'/images/file.png'));
	
	foreach ($variables['context'] as $child) {
		$author = $child->properties['cmis:createdBy'];
		$updated = date_format(date_create($child->properties['cmis:lastModificationDate']),'n/j/Y g:i A');
		
		switch ($child->properties['cmis:baseTypeId']) {
			case 'cmis:folder':
				$icon = $folder_img;
				$linkTitle = 
				$link = l($child->properties['cm:title'] ? $child->properties['cm:title'] : $child->properties['cmis:name'],'cmis_link/browser'.$child->properties['cmis:path']);
				$mimetype = 'Folder';
				$size = '';
				break;
			default:
				$icon = $document_img;
				$link = l($child->properties['cmis:name'], 'cmis_link/document/select', array(
						'attributes' => array('class' => array( 'cmis_link_document_select')),
				        'query' => array('id' => substr($child->id, 0, strpos($child->id, ';')))));
				$mimetype = $child->properties['cmis:contentStreamMimeType'];
				$size = number_format($child->properties['cmis:contentStreamLength'] /1000, 2, '.', ',') . 'K';
		}
		
		$rows[] = array($icon . ' '. $link,$mimetype,$size,$author,$updated);
	}
	return theme('table', array('header' =>$header, 'rows' =>$rows,'attributes' => array('class' => array('cmis_link_browse_children'))));
}


function theme_cmis_link_browse_breadcrumb($variables) {
	$next_img = theme('image', array('path' => drupal_get_path('module', 'cmis_link') .'/images/next.gif')); //placeholder image from cmis browser
	$contents = '';
	$root = array_shift($variables['bcarray']);
	$currentpath ='/'.$root;
	
	$bcarray = $variables['bcarray'];
	
	$contents .= '<div id="cmis-link-breadcrumb">';
	$contents .= l($root, 'cmis_link/browser'.$currentpath);
	
	if(sizeof($bcarray) >= 2) {
		 $contents .= $next_img .' ';
		 $currentpath .= '/' . $bcarray[0];
		 $linkName = $bcarray[0] . ' / ' . $bcarray[1];
		 $contents .= l($linkName,'cmis_link/browser'.$currentpath);
		 $currentpath .= '/' . $bcarray[1];
		 array_splice($bcarray,0,2);
	}
	
	foreach ($bcarray as $space) {
		$contents .= $next_img .' ';
		$currentpath .= '/'. $space;
		$pagelink = l($space, 'cmis_link/browser'.$currentpath);
		$contents .= $pagelink;
	}
	$contents .= '</div>';
	
	return $contents;
	
}

function theme_cmis_link_document_confirm($variables) {
	//Note: function not currently called
	$contents = '';
	
	$contents .= '<div id="cmis-link-document-confirm">';
	$contents .= '<h3>Placeholder - Confirm Document Selection</h3>';
	$contents .= '<br/><button type="button">Confirm</button>';
	$contents .= '</div>';
	
	return $contents;
}

function theme_cmis_link_folder_unavailable($variables) {
		
	$folderName1 = $variables['context']['folderName1'];
	$folderName2 = $variables['context']['folderName2'];
	$pathName = $variables['context']['pathName'];
	$siteName = $variables['context']['siteName'];

	$contents = '';
	
	$contents .= '<div id="cmis-link-return"><a href="/cmis_link/browser/Sites">Back to Sites</a></div>';
		
	$contents .= '<div id="cmis-link-folder-unavailable">';
	
	$contents .= '<h3>';
	$contents .= t('Error - Expected Public Folders in @siteName are unavailable', array('@siteName' => $siteName));
	$contents .= '</h3>';
	
	$contents .= '<p>';
	$contents .= t('The CMIS Link File Browser cannot access the @folderName1 folder at @pathName/@folderName1, nor can it access @folderName2 at  @pathName/@folderName1/@folderName2.', array('@pathName' => $pathName, '@folderName1' => $folderName1, '@folderName2' => $folderName2));
	$contents .= '<br/>';
	
	$contents .= t('This could be because:');
	
	$contents .='<ul>';
	$contents .='<li>'.t('@folderName1 and/or @folderName2 doesn\'t exist in the Repository', array('@folderName1' => $folderName1, '@folderName2' => $folderName2)).'</li>';
	$contents .='<li>'.t('@folderName1 and/or @folderName2 is misspelt or isn\'t named according to conventions', array('@folderName1' => $folderName1, '@folderName2' => $folderName2)).'</li>';
	$contents .='<li>'.t('@folderName1 and/or @folderName2 doesn\'t have group \'EVERYONE\' set as a Site Consumer in the folder\'s \'manage permissions...\' section (this may be intentional for @folderName1)', array('@folderName1' => $folderName1, '@folderName2' => $folderName2)).'</li>';
	$contents .='</ul>';
	
	$contents .= '<br/>';
	
	$contents .= t('For troubleshooting, the location of \'@folderName1\' can typically found in the Alfresco Repository from your Dashboard at:',array('@folderName1' => $folderName1));
	$contents .= '<br/>';
	$contents .= t('&gt; Repository').'<br/>';
	$contents .= t('&nbsp;&nbsp;&gt; Sites').'<br/>';
	$contents .= t('&nbsp;&nbsp;&nbsp;&nbsp;&gt; @siteName',array('@siteName' => $siteName));
	
	$contents .= '<br/><br/>';
	
	$contents .= t('\'@folderName2\' should respectively be found at:',array('@folderName2' => $folderName2));
	$contents .= '<br/>';
	$contents .= t('&gt; Repository').'<br/>';
	$contents .= t('&nbsp;&nbsp;&gt; Sites').'<br/>';
	$contents .= t('&nbsp;&nbsp;&nbsp;&nbsp;&gt; @siteName',array('@siteName' => $siteName)). '<br/>';
	$contents .= t('&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&gt; @folderName1',array('@folderName1' => $folderName1));
	
	$contents .= '</div>';
	
	return $contents;
}

function theme_cmis_link_document_upload($variables = null) {
    // this function is currently not used
	$contents = '';
	$contents .= drupal_render((drupal_get_form('cmis_link_upload_form')));
	return $contents;
}