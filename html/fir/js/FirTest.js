/***************
 * CONSTRUCTOR *
 **************/
function FirTest() 
{
    
  // Init map 
  var options = {
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
};

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
};

FirTest.prototype.drawFirPath = function()
{
  try
  {
    // Get JSON
    var json = jQuery.parseJSON( $("#taCoords").val() );

    // Init fir values
    var coefs = this.parseFirCoefs();
    this.laCount = 0;
    this.loCount = 0;
    var lat = parseFloat( json[0].latitude );
    var lng = parseFloat( json[0].longitude );
    // Init buffer with first lat/lng value
    var rawLats = this.getArray( coefs.length, lat );
    var rawLngs = this.getArray( coefs.length, lng );
		
    // Clear all previous paths
    this.clearMap(false,true);
		
    // Create fir path
    var path = [];
    for(var i in json)
    {
      var o = json[i];
			
      // Get FIR filtered lat/lng
      lat = parseFloat(o.latitude);
      lng = parseFloat(o.longitude);
      lat = this.getFirValue( lat, rawLats, coefs, this.laCount++ );
      lng = this.getFirValue( lng, rawLngs, coefs, this.loCount++ );
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
};

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
      result.push(coef);
  }
  return result;
};

FirTest.prototype.getArray = function(len,val)
{
  var arr = new Array(len);
  while(--len >= 0) { arr[len] = val; }
  return arr;
}

FirTest.prototype.getFirValue = function(rawVal, rawArr, coefsArr, count)
{
  // Store raw data
  rawArr[count] = rawVal;

  var len = coefsArr.length;
  var result = 0;
  var index = count;
  for(var i=0; i<len; i++)
  {
    result += coefsArr[i] * rawArr[index--];
		
    if( index < 0 )
      index = ( len - 1 );
  }

  if( ++count >= len )
    count = 0;

  return result;
};

/****************
 * MISC METHODS *
 ***************/
FirTest.prototype.clearRaw = function(clearCoords,clearCoefs)
{
  $("#taCoords").val("");
  this.clearMap(true,false);
};
