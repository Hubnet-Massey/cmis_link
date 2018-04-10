CONTENTS OF THIS FILE
---------------------
   
 * Introduction
 * Requirements
 * Installation
 * Configuration


INTRODUCTION
------------

The CMIS Link Module is a repository browser that facilitates access to Alfresco repositories via CMIS to provide easier access to publicly available documents in Alfresco. It contains two additional submodules that take advantage of the CMIS Link repository browser to facilitate embedding CMIS Content URLs from Alfresco into Drupal entities: 

 * The CMIS Link CKEditor submodule is an extension to the CKEditor module that provides an additional link type to the CKEditor, enabling users to browse and select files in CMIS Link Repository browser for linking in the CKEditor. 

 * The CMIS Link Field submodule is a Drupal field implementation defining a CMIS link field type, which provides aceess to the CMIS Link Repository browser during content creation end editing, providing a link for file access while viewing content. 


REQUIREMENTS
------------

The CMIS Link Module requires the following modules:

 * CMIS (https://drupal.org/project/cmis )
 * CMIS Browser (submodule of CMIS module)
 
Additionally, The CMIS Link CKEditor submodule requires the following modules: 

 * CKEditor (https://drupal.org/project/ckeditor)

INSTALLATION
------------

 * Install as you would normally install a contributed Drupal module. 
Visit: https://drupal.org/documentation/install/modules-themes/modules-7 
for further information. 


CONFIGURATION
-------------

For CMIS Link Module:

 * Verify that required modules are installed and configured correctly
  - CMIS
  - CMIS Browser
 
 * Activate the CMIS Link Module
 
 * Under Administration » People » Permissions enable 'Access CMIS Link File Browser' for user roles intended to browse the file repository
 
 * Under Administration » People » Permissions enable 'Acces CMIS Data' for user roles intended to view/download files
 
For CMIS Link CKEditor Submodule:

 * Verify that required modules are installed and configured correctly
  - CMIS
  - CMIS Browser
  - CKEditor
 
 * Activate the CMIS CKEditor Submodule
 
 * Under Administration » Configuration » CKEditor » Profiles, for each profile you wish to enable the submodule for, edit and do the following:
  - Editor Appearance » Plugins, select CMIS Link File Browser and save
  
For CMIS Link Field Submodule

 * Verify that required modules are installed and configured correctly
  - CMIS
  - CMIS Browser
  
 * Activate the CMIS Link Field Submodule


ADDITIONAL NOTES
----------------

 * The CMIS Link Browser is hard-coded to look only in the Sites subfolder of the root directory of the alfresco repository connected via CMIS, and will look only in the DocumentLibrary of Public Sites, and will attempt to find a public Subfolder 'Publish to Web' within DocumentLibrary if the DocumentLibrary is not available. An error message will be displayed if the user attempts to access a site where both options are inaccesible to the current user. The user needs to be a member of the site in alfresco to access to the site library via CMIS Link File Browser. If you encounter errors in being unable to access a folder or document, please check the following:
  - The folder exists in the repository
  - The folder is named correctly according to conventions
  - The folder has group 'EVERYONE' set as role Consumer or Site Consumer in its permissions
  - The file exists in the repository

 * The CMIS link uses the CMIS user credentials in 'cmis_repositories' to send request to the people api (to get user sites).
  - The CMIS user only gets public sites the the current user belongs to unless CMIS user has admin permissions.
  
 * when editing Drupal pages, ensure that 'text format' is the same as the profile you enabled the plugin for, or the button won't work.
 

TODO
----

Icons used in cmis_link/images are placesholder and are currenty sourced from the CMIS Browser Module. Properly sourced or original icons are needed before full publication.
