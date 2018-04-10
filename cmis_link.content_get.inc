<?php

/**
 * 
 * 
 */
function cmis_link_content_get() {
	module_load_include ( 'api.inc', 'cmis' );
	
	if(!isset($_SESSION['cmis_link'])){
		$_SESSION['cmis_link'] = array();
	}
	$cmis_link_session = & $_SESSION['cmis_link'];
	
	try {
		$repository = cmis_get_repository ();
		$object = _cmis_link_content_object_from_request ( $repository );
		
		switch ($object->properties ['cmis:baseTypeId']) {
			case 'cmis:document' :
				return _cmis_link_content_get_document ( $repository, $object );
				break;
			case 'cmis:folder' :
				return _cmis_link_content_get_folder ( $repository, $object, array_slice ( explode ( '/', $_GET ['q'] ), 2 ) );
				break;
			default :
				throw new CMISException ( t ( 'Unable to handle cmis object @object_id of type @object_type', array (
						'@object_id' => $object->id,
						'@object_type' => $object->type 
				) ) );
		}
		
		
	} catch ( CMISException $e ) {
		cmis_error_handler ( 'cmis_link', $e );
		return '';
	}
}

function _cmis_link_content_get_document($repository, $object) {
	//Note: function currently not called -
	error_log('_cmis_link_content_get_document');
	$cmis_link_session = & $_SESSION['cmis_link'];
	
	error_log('$object: '.var_export($object,true));
	$document = array('id' => $object->id, 'name' => $object->properties['cmis:name'], );
	error_log('$document: '.var_export($document,true));
	
	drupal_add_js(drupal_get_path('module','cmis_link'). '/scripts/popup.js');
	drupal_add_js(array('cmis_link' => array('document' => array('id' => $object->id, 'name' => $object->properties['cmis:name'], ))), 'setting');
	
	$cmis_link_session['document'] = $document;
	return theme ('cmis_link_document_confirm',array(
			'document' => $document,			
	));
}

function _cmis_link_content_get_folder($repository, $object) {
	$getDocumentlibrary = false;
	$filterSites = false;
	
	try {
		$objectPath = explode('/',$object->properties['cmis:path']);
		
		if ((sizeof($objectPath) == 3 ) && strcmp($objectPath[1],'Sites') == 0) {
			//get the child object; parent == Sites, object = sitename, child = documentLibrary;
			$getDocumentlibrary = true;
		} elseif ((strcmp($object->properties['cmis:name'],'Sites') == 0) && (strcmp($object->properties['cmis:path'],'/Sites')) == 0) {
			$filterSites = true;
		}
		
		$children = cmisapi_getChildren($repository->repositoryId,$object->id)->objectList;
		//right now in development environment  'sites' is empty -in prod. environment the result will be an array of sitenames.
		if($filterSites) {
			$tempArray = array();
			if(!isset($_SESSION['user_profile']['sites'])){
				_cmis_link_content_get_user_sites($repository);
			}
			$sites = $_SESSION['user_profile']['sites'];
			
			foreach ($children as $child) {
				if (in_array($child->properties['cmis:name'],$sites)){
					array_push($tempArray,$child);
				}
			}
			$children = $tempArray;
		}
		
		if ($getDocumentlibrary) {
			$documentLibrary = null;
			foreach ( $children as $child ) {
				//attempt to find documentLibrary
				if(strcmp($child->properties['cmis:name'],'documentLibrary') == 0) {
					$documentLibrary = $child;
				}
			}
			if(!is_null($documentLibrary)) {			
				return _cmis_link_content_get_folder($repository,$documentLibrary,array_slice(explode('/',$_GET['q']),2));
				exit();
				
			}else {
				//attempt to find Publish to Web
				$statement = "SELECT F.* FROM cmis:folder F WHERE IN_TREE(F,'$object->id') AND F.cmis:name LIKE 'Publish to Web'";
				
				$queryResults = cmisapi_query($repository->repositoryId,$statement);
				
				
				$publishToWeb = null;
				
				foreach ($queryResults->objectList as $resultObject){
					$publishToWebPath = $resultObject->properties['cmis:path'];
					if (strcmp($publishToWebPath,implode('/',$objectPath).'/documentLibrary/Publish to Web') == 0) {
						$publishToWeb = $resultObject;
						return  _cmis_link_content_get_folder($repository,$publishToWeb,array_slice(explode('/',$_GET['q']),2));
						exit();
					}
				}			
				//Error page for unavailable documentLibrary & Publish to Web (absent/mislabeled or 'EVERYONE' permissions aren't enabled properly for either)
				drupal_add_css(drupal_get_path('module','cmis_link').'/css/popup_browser.css');
				$strings = array('folderName1' =>'documentLibrary', 'folderName2' => 'Publish to Web', 'pathName' =>$object->properties['cmis:path'], 'siteName' => $objectPath[2]);
				print theme('cmis_link',array('content'=> theme('cmis_link_folder_unavailable',array('context' => $strings))));
				exit();
			}
		}
	} 
	catch (Exception $e) {
		cmis_error_handler('cmis_link', $e);
		return '';
	}
	drupal_add_css(drupal_get_path('module','cmis_link').'/css/popup_browser.css');
	drupal_add_js(drupal_get_path('module','cmis_link'). '/scripts/selectCMISLinkDoc.js');
	 
	print theme('cmis_link', array(
			'content' => _cmis_link_content_get_theme_content($children,explode('/',substr($object->properties['cmis:path'],1))),
	));
	exit();
}

function  _cmis_link_content_object_from_request($repository) {
	$object_id = NULL;
	$object_path = NULL;
	$bcarray = array_slice(explode('/',$_GET['q']),2);
	if (count($bcarray) == 0) {
		$bcarray = array_slice(explode('/', 'Sites/'),0); //Will test this to see if this works -should start the user in Sites Subfolder
	}
	if (array_key_exists('id',$_GET)) {
		$object_id = urldecode($_GET['id']);
	}
	elseif(!empty($bcarray)) {
		$object_path = drupal_encode_path('/'.implode('/',$bcarray));
	}
	elseif (array_key_exists('browser_default_folderId', $repository->settings)) {
		$object_id = $repository->settings['browser_default_folderId'];
	}
	elseif (array_key_exists('browser_default_folderPath',$repository->settings)) {
		$object_path =drupal_encode_path($repository->settings['browser_default_folderPath']);
	}
	else {
		$object_id = $repository->info->repositoryInfo['cmis:rootFolderId'];
	}
	if (!is_null($object_id)) {
		$object = cmisapi_getProperties($repository->repositoryId,$object_id);
	}
	elseif (!is_null($object_path)) {
		$object = cmisapi_getObjectByPath($repository->repositoryId, $object_path);
	}
	else {
		throw new CMISException ('Unknown CMIS Object');
	}
	
	return $object;
}

function _cmis_link_content_get_theme_content($children, $bcarray) {
	$content = '';
	
	//add children, breadcrumb helper functions
	$content .= theme('cmis_link_browse_breadcrumb', array('bcarray' => $bcarray));
	$content .= theme('cmis_link_browse_children', array('context' => $children));
	//$content .= theme('cmis_link_document_upload', array());
	return $content;
}

/**
 * This helper function helps to get a colleciton of the sites a user belongs to.
 * It uses the cmis user credentials in 'cmis_repositories' to send request to the people api.
 * The cmis user only gets public sites the the current user belongs to unless cmis user has admin permissions.
 */
function _cmis_link_content_get_user_sites($repository){
	global $user;
	// gets alfresco repo url and modify it to get a colleciton of the sites a user belongs to.
	$temp_url = explode("api", $repository->settings['url'], 2);
	$alfresco_api_base = $temp_url[0];
	$_SESSION['user_profile']['sites'] = array();
	$url = $alfresco_api_base . "service/api/people/" . urlencode($user->name) . "/sites";

	$ch = curl_init();
	if(isset($repository->settings['user']) && isset($repository->settings['password'])){
		$service_user = $repository->settings['user'].':'.$repository->settings['password'];
		curl_setopt($ch, CURLOPT_USERPWD, $service_user);
	}
	curl_setopt($ch, CURLOPT_TIMEOUT, 30);
	curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
	curl_setopt($ch, CURLOPT_URL, $url);
	$return = curl_exec($ch);

	$httpCode = curl_getinfo($ch, CURLINFO_HTTP_CODE);
	curl_close($ch);

	if ($httpCode >= 400) {
		// make it easier to debug HTTP authentication required etc
		if ($httpCode == 401) {
			watchdog('CMIS Link Get Sites', '%url returned HTTP %code Unauthorized: This request requires HTTP authentication', array('%url' => $url, '%code' => $httpCode), WATCHDOG_ERROR);
		} else {
			watchdog('CMIS Link Get Sites', '%url returned HTTP %code: %response', array('%url' => $url, '%code' => $httpCode, '%response' => $return), WATCHDOG_ERROR);
		}
	} else if (!$return) {
		watchdog('CMIS Link Get Sites', 'Empty JSON response for %name', array('%name' => $user->name), WATCHDOG_ERROR);
	} else {
		$json = json_decode($return);
		if (!$json) {
			watchdog('CMIS Link Get Sites', 'Invalid JSON response for %name: %response', array('%name' => $user->name, '%response' => $return), WATCHDOG_ERROR);
		} else {
			foreach ($json as $site) {
				if (isset($site->shortName)) {
					$_SESSION['user_profile']['sites'][] = $site->shortName;
					// other fields available: url, sitePreset, title, description, siteManagers, isPublic, visibility, ...
				}
			}
		}
	}
}