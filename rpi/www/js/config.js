function Config()
{
	// TODO: Change this
	cfg = config;
}

Config.prototype.init = function()
{
	var cfg = config;
	
	var html = '';
	for(var name in cfg)
	{
		var value = cfg[name];
		html += '<div class="header">'+name+'</div>';
		html += this.getHtml(value);
		html += '</div>';
	}
	$("#content").append( html);
}

Config.prototype.save = function()
{
	this.setJson(cfg);
	// TODO: Save
}

Config.prototype.setJson = function(o)
{
	for(var name in o)
	{
		var value = o[name];
		if( typeof value == "object" )
			this.setJson(value);
		else
		{
			var e = $("#"+name);
			if( e && e.val() && e.val() != "" )
			{
				console.log("Setting "+name+" to "+e.val());
				value = e.val();
			}
		}
	}
}

Config.prototype.getHtml = function(o)
{
	var html = '';
	for(var name in o)
	{
		var value = o[name];
		html += '<div class="row">';
		html += ' <div class="name">'+name+'</div>';
		
		if( typeof value == "object")
		{
			html += '<div class="value sub">';
			html += this.getHtml(value);
			html += '</div>';
		}
		else
			html += ' <input class="value input-text" type="text" id="'+name+'" value="'+value+'"/>';
			
		html += '</div>';
	}
	return html;
}
