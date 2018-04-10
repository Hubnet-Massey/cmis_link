jQuery(function($) {
	$('#cmis-link-field-browse-button').click(function(event) {
		event.preventDefault();
		sessionStorage.setItem('cmis_link_field', 'cmis_link_field');
		
		var hSize = screen.width*0.7;
		var vSize = screen.height*0.7;
		var left = (screen.width - hSize)/2;
		var top = (screen.height - vSize)/2;
		var windowSize = 'height=' + vSize + ',width=' + hSize +',left=' + left + ',top=' + top;
		var options = 'modal=yes,scrollbars=yes,'+ windowSize;
		var popup;	
		var href = document.getElementById("cmis-link-field-browse-button").getAttribute("href");
		if ( href.charAt(0) === '#' || 0 === href.length) {
			popup = window.open(Drupal.settings.basePath + 'cmis_link/browser', 'CMIS Link Field Browser', options);
		}
		else{
			    popup = window.open(href, 'CMIS Link Field Browser', options);
		}
		popup.focus();
		popup.onunload = function(){
			  // DOM unloaded, so the window is likely closed.
			sessionStorage.removeItem('cmis_link_field');
			var document_path = document.getElementById("cmis-link-field-document-path").value;
			document.getElementById("cmis-link-field-browse-button").setAttribute("href",document_path);
		}
	});
	window.selectCmisLinkFieldDoc= function(file_link,breadcrumbs,document_path) {
		$.ajax({
			type: 'GET',
			url: file_link,
			dataType: 'json',
			success: function (result) {
				if (!(result == null)){
					var document_link = Drupal.settings.basePath + 'cmis/browser/?id=' + result.id.split(';')[0];
					var document_name = result.name;
					
					console.log(result);
					
					document.getElementById("cmis-link-field-display-name").value = document_name;
					document.getElementById("cmis-link-field-breadcrumbs").value = '<div id="library-breadcrumbs">' + breadcrumbs+'<div id="selected-file-name"><img typeof="foaf:Image" class="img-responsive" src="'+Drupal.settings.basePath+'sites/all/modules/cmis_link/images/next.gif" alt="">'+document_name+'</div></div>';
					document.getElementById("cmis-link-field-document-path").value = document_path;
					document.getElementById("cmis-link-field-document-link").value = document_link;
					
					// replace 'Browse Library' button path to the document link
				    var cmis_link_browse_button = document.getElementById('cmis-link-field-browse-button');
					cmis_link_browse_button.setAttribute("href", document_path);
					
					if(document.getElementById('selected-file-name')){
						var ex_file_name = document.getElementById('selected-file-name');
						document.getElementById('library-breadcrumbs').removeChild(ex_file_name);
					}
					
					// set breadcrumbs with the original file name
					var breadcrumbs_div = document.getElementById('library-breadcrumbs');
					breadcrumbs_div.innerHTML = breadcrumbs;
					
					var new_filename = document.createElement("div");
					new_filename.innerHTML = '<img typeof="foaf:Image" class="img-responsive" src="'+Drupal.settings.basePath+'sites/all/modules/cmis_link/images/next.gif" alt="">'+document_name;
					new_filename.setAttribute("id", "selected-file-name");
					breadcrumbs_div.appendChild(new_filename);
					
					sessionStorage.removeItem('cmis_link_field');
				}
			},
			error: function (xmlhttp) {
				alert(Drupal.ajaxError(xmlhttp,file_link));
				sessionStorage.removeItem('cmis_link_field');
			}
		});
	};
	// use button click as the default click event for the links in breadcrumbs
	$(document).on("click", "#cmis-link-breadcrumb a", function() {
		$("#cmis-link-field-browse-button").attr("href", this.href);
		$('#cmis-link-field-browse-button').trigger('click');
		return false;
    });   
});