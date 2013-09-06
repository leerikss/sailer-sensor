function SailerSensor()
{
	MARGIN = 20;
}

SailerSensor.prototype.init = function()
{
	// Init nav
	$("li").click(function(e) {
		e.preventDefault();
		$("li").removeClass("selected");
		$(this).addClass("selected");
		$("#content").attr("src", $(this).children(":first").attr("href") );
	});
	
	// Set iframe height
	var resize = function() {
		$("#content").css("height", $(window).height() - $("#top").height() - this.MARGIN );
	};
	resize();
	$(window).resize(resize);
}