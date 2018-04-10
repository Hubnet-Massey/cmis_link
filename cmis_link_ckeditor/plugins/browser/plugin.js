/**
 * 
 */
(function($) {
	//helper functions go here
	window.selectCmisCkeditorLinkDoc = function(link) {
		$.ajax({
			type: 'GET',
			url: link,
			dataType: 'json',
			success: function (result) {
				if (!(result == null)){
					setLinkDoc(result);
					var URLText = CKEDITOR.dialog.getCurrent().getContentElement('info','cmis_link_ckeditor_url');
					var docNameText = CKEDITOR.dialog.getCurrent().getContentElement('info','cmis_link_ckeditor_text');
					URLText.setValue(Drupal.settings.basePath + 'cmis/browser/?id=' + linkDoc.id.split(';')[0]);
					docNameText.setValue(linkDoc.name);
					UrlRetrieved = true;
				}
			},
			error: function (xmlhttp) {
				alert(Drupal.ajaxError(xmlhttp,link));
			}
		});

	};

	var getById = function (array, id, recurse) {
		for (var i = 0, item;(item = array[i]); i++) {
			if (item.id == id) return item;
			if (recurse && item[recurse]) {
				var retval = getById(item[recurse], id, recurse);
				if(retval) return retval;
			}
		}
		return null;
	};

	var resetInitValues = function(dialog) {
		dialog.foreach(function(contentObj) {
			contentObj.setInitValue && contentObj.setInitValue();
		});
	};

	var linkDoc = null; var setLinkDoc = function(obj) {
		linkDoc = obj;
	}
	
	var originalURL = null; var UrlRetrieved = false;
	
	var hasPermissions = function() {
		if (typeof Drupal.settings.cmis_link_ckeditor === 'undefined' || Drupal.settings.cmis_link_ckeditor === null) {
			return false;
		} else {
			if (Drupal.settings.cmis_link_ckeditor.has_permissions === 'true') {
				return true;
			} else {
				return false;
			}
		}
	}
	CKEDITOR.plugins.add('cmis_link_ckeditor',{
		init:function (editor) {
			CKEDITOR.on('dialogDefinition',function(e) {
				if ((e.editor != editor) || e.data.name != 'link') return;

				var definition = e.data.definition;

				definition.onFocus = CKEDITOR.tools.override(definition.onFocus, function(original) {
					return function() {
						original.call(this);
						//customize onFocus here (if necessary)
						if (this.getValueOf('info','linkType') == 'cmis_link_ckeditor') {
							this.getContentElement('info','cmis_link_ckeditor_url').focus();
						} 
					}
				});

				definition.onOk = CKEDITOR.tools.override(definition.onOk, function(original) {
					return function() {
						var process = false;
						if (hasPermissions()) {
							var url = this.getContentElement('info', 'cmis_link_ckeditor_url').getValue();
							if ((this.getValueOf('info', 'linkType') == 'cmis_link_ckeditor') && url != originalURL) {
								var ranges = editor.getSelection().getRanges(true); 
								if((ranges.length == 1)) {
									process = true;
								}
							}
						}
						original.call(this);
						if(process) {
							var link = CKEDITOR.plugins.link.getSelectedLink(editor);
							var name = this.getContentElement('info', 'cmis_link_ckeditor_text').getValue();
							var openTag = null, closeTag = null; var enclosingTags = false;
							if (link) {
								var linkHtml = link.getHtml();
								var tags = [];
								tags[0] = /^<[^<>]+>/g.exec(linkHtml);
								tags[1] = /<\/[^<>]+>$/g.exec(linkHtml);
								if (tags[0]) {
									openTag = tags[0][0];
								}
								if (tags[1]) {
									closeTag = tags[1][0];
								}								
								if (openTag && closeTag && openTag.slice(1,-1) == closeTag.slice(2,-1)) {
									enclosingTags = true;
								}							
								if(link.getText() == url && name.length >= 1){
									if (enclosingTags) {
										link.setHtml(openTag + name + closeTag);
									} else {
										link.setText(name);
									}
								}
							}
							CKEDITOR.plugins.link.getSelectedLink(editor).setCustomData('name',this.getContentElement('info', 'cmis_link_ckeditor_text').getValue());
						}						
						setLinkDoc(null);
						originalURL = null;
						UrlRetrieved = false;
					}
				});
				
				var linkTypeName = 'Library Link'; var buttonName = 'Browse Library'; var textareaName = 'Document Name'; var linkURL = 'Link URL';
				
				if(hasPermissions()) {
					linkTypeName = Drupal.settings.cmis_link_ckeditor.link_type_name;
					buttonName = Drupal.settings.cmis_link_ckeditor.button_name;
					textareaName = Drupal.settings.cmis_link_ckeditor.textarea_name;
					linkURL = Drupal.settings.cmis_link_ckeditor.linkURL_name;
				}
				
				
				var infoTab = definition.getContents('info');
				var content = getById(infoTab.elements, 'linkType');
				if (hasPermissions()) {
					content.items.unshift([linkTypeName, 'cmis_link_ckeditor']) //should pass translated string through drupal.settings here
					infoTab.elements.push({
						type: 'vbox',
						id: 'cmisLinkOptions',
						children: [
							{
								type:'text',
								id: 'cmis_link_ckeditor_url',
								label: linkURL,
								'default': '',
								validate: function() {
									var dialog = this.getDialog();
									var urlValue = this.getValue();
									if(dialog.getValueOf('info', 'linkType') != 'cmis_link_ckeditor') {
										return true;
									}
									if(urlValue == '') {
										alert('Please select a Document');
										this.focus;
										return false;
									}
									return true;
								}
							},
							{
								type:'text',
								id: 'cmis_link_ckeditor_text',
								label: textareaName,
								'default': '',
							},
						    {
							type: 'button',
							id: 'cmis_link_ckeditor_browser',
							label: buttonName,
							onClick: function() {
								//this is called when the button is clicked
								//open a browser window or new dialog here
								var hSize = screen.width*0.7;
								var vSize = screen.height*0.7;
								var left = (screen.width - hSize)/2;
								var top = (screen.height - vSize)/2;
								var windowSize = 'height=' + vSize + ',width=' + hSize +',left=' + left + ',top=' + top;
								var options = 'modal=yes,scrollbars=yes,'+ windowSize; 
								var popup = window.open(Drupal.settings.basePath + 'cmis_link/browser', 'CMIS Link Browser', options);
								popup.focus();
							},
							onLoad: function() {
							},
							setup: function(data) {
							}
						}]
					});
				}

				content.onChange = CKEDITOR.tools.override(content.onChange, function(original) {
					return function() {
						original.call(this);
						//customize onChange here
						var dialog = this.getDialog();
						if (hasPermissions()) {
							var element = dialog.getContentElement('info', 'cmisLinkOptions').getElement().getParent().getParent();
							if(this.getValue() == 'cmis_link_ckeditor') {
								element.show();
								var targetTab = dialog.definition.getContents('target');
								if(targetTab && !targetTab.hidden) {
									dialog.hidePage('target');
								}
								var uploadTab = dialog.definition.getContents('upload');
								if(uploadTab && !uploadTab.hidden) {
									dialog.hidePage('upload');
								}
							} else {
								element.hide();
							}
						}
						//write a conditional to check for alternate elements? - lower priority for now
					};

				});

				content.setup = function(data) {
					if(hasPermissions()) {
						if(!data.type || (data.type == 'url') && !data.url) {
							data.type = 'cmis_link_ckeditor';
						}
						else if (data.url && !data.url.protocol && data.url.url) {
							//set up 'edit link' detection here						
							data.type = 'cmis_link_ckeditor';
							var dialog = this.getDialog();
							delete data.url;
							resetInitValues(dialog);
							dialog.setupContent(data);
							//currently deletes existing value and forces the process to repeat over again.
						}
						//customize setup here
						if (CKEDITOR.plugins.link.getSelectedLink(CKEDITOR.currentInstance) && CKEDITOR.plugins.link.getSelectedLink(CKEDITOR.currentInstance).getAttribute('href')){
							var href = CKEDITOR.plugins.link.getSelectedLink(CKEDITOR.currentInstance).getAttribute('href');
							var dataName = CKEDITOR.plugins.link.getSelectedLink(CKEDITOR.currentInstance).getCustomData('name');
							this.getDialog().getContentElement('info','cmis_link_ckeditor_url').setValue(href);
							if(dataName) {
								this.getDialog().getContentElement('info','cmis_link_ckeditor_text').setValue(dataName);
							}
							originalURL = href;
						}
					} else {
						var dialog = this.getDialog();
						var element = dialog.getContentElement('info', 'urlOptions').getElement().getParent().getParent();
						if (this.getValue() == 'url') {
							element.show();
							if (editor.config.linkShowTargetTab) {
								dialog.showPage('target');
							}
							var uploadTab = dialog.definition.getContents('upload');
							if(uploadTab && !uploadTab.hidden) {
								dialog.hidePage('upload');
							}
						} else {
							element.hide();
						}
					}
					this.getDialog().getContentElement('info','cmis_link_ckeditor_text').disable();
					this.setValue(data.type || 'url');
				};

				content.commit = CKEDITOR.tools.override(content.commit, function(original) {
					return function(data) {
						original.call(this,data);
						//customize commit here
						if (data.type == 'cmis_link_ckeditor'){
							if(!(linkDoc == null)) {
								var id = linkDoc.id.split(';')[0];
								data.type = 'url';
								var dialog = this.getDialog();
								dialog.setValueOf('info','protocol','');
								dialog.setValueOf('info','url', CKEDITOR.dialog.getCurrent().getContentElement('info','cmis_link_ckeditor_url').getValue());
							}else if (this.getDialog().getContentElement('info','cmis_link_ckeditor_url').getValue() != '') {
								data.type = 'url';
								var dialog = this.getDialog();
								dialog.setValueOf('info','protocol','');
								dialog.setValueOf('info','url', CKEDITOR.dialog.getCurrent().getContentElement('info','cmis_link_ckeditor_url').getValue()); 
							}
						}
					}
				});
			});
		}
	});
})(jQuery);