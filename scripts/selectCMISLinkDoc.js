(function($) {

	if(sessionStorage.getItem('cmis_link_field') == 'cmis_link_field'){
		var updateCmisLinkFieldDoc = function(){
			$("a.cmis_link_document_select").click(function (e){
				e.preventDefault();
				return false;
			});		
			$("a.cmis_link_document_select").on("dblclick", function(e) {
				var file_link = $(this).attr("href");
				var breadcrumbs = $('#cmis-link-breadcrumb')[0].outerHTML;
				var document_path = $('#cmis-link-breadcrumb')['context'].URL;
				if (opener && opener.selectCmisLinkFieldDoc) {
				opener.selectCmisLinkFieldDoc(file_link,breadcrumbs,document_path);
				window.close();
			} else {
				e.preventDefault();
			}
				return false;
			});
		};
		$(function() {
			updateCmisLinkFieldDoc();
		});
	}
	else{
		var updateCkeditorLinkDoc = function(){
				$("a.cmis_link_document_select").click(function (e){
					e.preventDefault();
					return false;
				});
				
				$("a.cmis_link_document_select").on("dblclick", function(e) {
					var link = $(this).attr("href");
					if (opener && opener.selectCmisCkeditorLinkDoc) {
					opener.selectCmisCkeditorLinkDoc(link);
					window.close();
				} else {
					e.preventDefault();
				}
					return false;
				});
			
		};
		$(function() {
			updateCkeditorLinkDoc();
		});
	}
})(jQuery);