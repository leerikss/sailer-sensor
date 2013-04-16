/***************
 * CONSTRUCTOR *
 **************/
function FirTest() {

	// Init map 
	var options = 
	{
		mapTypeId: google.maps.MapTypeId.ROADMAP,
		center: new google.maps.LatLng(60.169845,24.938551), // <-- center to HELSINKI perkele!
		scaleControl: true,
		zoom: 18
	};
    this.map = new google.maps.Map( $("#map")[0], options );
	this.rawLine = new google.maps.Polyline([]);
	this.firLine = new google.maps.Polyline([]);
	
	// Map listener
	var _this = this;
	google.maps.event.addListener(this.map, "click", 
		function(event) { _this.mapClicked(event); _this.drawRawPath(); } );
		
	// Init textarea event
	$("textarea").focus(function() {
    	var $this = $(this);
    	$this.select();	
	    // Work around Chrome's little problem
	    $this.mouseup(function() {
    	    // Prevent further mouseup intervention
    	    $this.unbind("mouseup");
        	return false;
    	});    	
    });
}

/***************
 * MAP METHODS *
 **************/
FirTest.prototype.drawRawPath = function()
{
	try
	{
		var json = jQuery.parseJSON( $("#taCoords").val() );
		
		// Clear all previous paths
		this.clearMap(true,true);
		
		// Create path
		var bounds = new google.maps.LatLngBounds();
		var path = [];
		for(var i in json)
		{
			var o = json[i];
			var ll = new google.maps.LatLng( o.latitude, o.longitude );
			path.push( ll );
			bounds.extend( ll );
		}
		
		// Draw new path
		this.rawLine = new google.maps.Polyline({
			path: path,
			strokeColor: '#0000FF',
			strokeOpacity: 0.5,
			strokeWeight: 4,
			map: this.map
        });
	}
	catch(e)
	{
		alert("Unable to parse JSON coords!");
		console.error(e.stack);
	}
}

FirTest.prototype.drawFirPath = function()
{
	try
	{
		// Init coefs
		var coefs = this.parseFirCoefs();
		var rawLats = this.getZeroFillArr( coefs );
		var rawLons = this.getZeroFillArr( coefs );
		var laCount = 0;
		var loCount = 0;
		
		// Get JSON
		var json = jQuery.parseJSON( $("#taCoords").val() );
		
		// Clear all previous paths
		this.clearMap(false,true);
		
		// Create fir path
		var path = [];
		for(var i in json)
		{
			var o = json[i];
			
			// Get FIR filtered lat/lng
			var lat = parseFloat(o.latitude);
			var lng = parseFloat(o.longitude);
			lat = this.getFirValue( lat, rawLats, coefs, laCount++ );
			lng = this.getFirValue( lng, rawLons, coefs, loCount++ );
			var ll = new google.maps.LatLng( lat, lng );
			path.push( ll );
		}		
		
		// Draw new path
		this.firLine = new google.maps.Polyline({
			path: path,
			strokeColor: '#FF0000',
			strokeOpacity: 0.5,
			strokeWeight: 4,
			map: this.map
        });
	}
	catch(e)
	{
	
		alert("Action failed");
		console.error(e.stack);
	}
}

FirTest.prototype.mapClicked = function(event)
{
	var v = $("#taCoords").val();

	// First time clear textarea
	if( !v.match(/\[/gi) )
		v = "";
		
	// Remove unwanted chars
	v = v.replace(/[\[\]]+/gi, "");
	
	// Add comma to previous json object
	v = (v != "") ? ( v + ',\n' ) : ""
	
	var nv = 
		'{\n'+
		' "latitude": "'+event.latLng.lat()+'",\n' +
		' "longitude": "'+event.latLng.lng()+'"\n' +
		'}';
		
	$("#taCoords").val( '[' + v + nv + ']'); 
}

FirTest.prototype.clearMap = function(clearRaw,clearFir)
{
	if(clearRaw)
		this.rawLine.setPath([]);
	if(clearFir)
		this.firLine.setPath([]);
}

/***************
 * FIR METHODS *
 **************/
FirTest.prototype.parseFirCoefs = function(coefs)
{
	var txt = $("#taCoefs").val();
	var arr = txt.split("\n");
	var result = [];
	for(var i=0; i<arr.length; i += 1)
	{
		var coef = parseFloat( arr[i] );
		if( !isNaN(coef) )
		{
			// console.log( "coef: " + coef );
			result.push(coef);
		}
	}
	return result;
}

FirTest.prototype.getZeroFillArr = function(arr)
{
	// Zero fill raw array
	var len = arr.length;
	var arr = new Array(len);
	while(--len >= 0) { arr[len] = 0; }
	return arr;
}

FirTest.prototype.getFirValue = function(rawVal, rawArr, coefsArr, c) 
{
	// Count must be within 0 and coefsArr.length
	var count = c;
	while(count >= coefsArr.length) { count -= coefsArr.length; }

	// Store raw data
	rawArr[count] = rawVal;
	
	// Return value as is if this is first round
	if( c < coefsArr.length-1)
		return rawVal;
		
	var result = 0;
	var index = count;
	for(var i=0; i<coefsArr.length; i++)
	{
		// console.log( "rawArr["+index+"]: " + rawArr[index] );
		// console.log( "coefsArr["+i+"]: " + coefsArr[i] );
		
		result += coefsArr[i] * rawArr[index--];
		
		if( index < 0 )
			index = ( coefsArr.length - 1 );
	}
	
	console.log( "rawVal: " +rawVal + "\t\tResult: " + result + "\t\tCount: "+ count );
	
	return result;
}

/****************
 * MISC METHODS *
 ***************/
 FirTest.prototype.clearRaw = function(clearCoords,clearCoefs)
{
	$("#taCoords").val("");
	this.clearMap(true,false);
}
