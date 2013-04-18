/***************************************
*  Add calculation methods to Numbers  *
***************************************/
Number.prototype.toRad = function() {  // convert degrees to radians
  return this * Math.PI / 180;
}

Number.prototype.toDeg = function() {  // convert radians to degrees (signed)
  return this * 180 / Math.PI;
};

Number.prototype.toBrng = function() {  // convert radians to degrees (as bearing: 0...360)
  return (this.toDeg()+360) % 360;
};

/***************
 * CONSTRUCTOR *
 **************/
function FirTest() 
{
  // Init map 
  var options = 
    {
    mapTypeId: google.maps.MapTypeId.ROADMAP,
    center: new google.maps.LatLng(60.169845,24.938551), // <-- center to HELSINKI perkele!
    scaleControl: true,
    zoom: 18
    };

  this.map = new google.maps.Map( $("#map")[0], options );
  this.rawPath = new google.maps.Polyline([]);
  this.firPath = new google.maps.Polyline([]);
  this.regPath = new google.maps.Polyline([]);

  // Map listener
  var _this = this;
  google.maps.event.addListener(
    this.map, "mouseup", 
    function(event) { 
      // Update textarea JSON
      _this.updateJson(event); 
      // Draw path on map
      _this.drawPathFromJson(); 
    });

  // Select all when textarea clicked
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
	
	// Init data object
	this.data = 
  {
    buffInit: true,
    buffLat: [],
    buffLon: [],
    buffSize: -1,
    firBuffSize: 5,
    coefs: this.parseFirCoefs(),
    latCount: 0,
    lonCount: 0,
  };
};

/***************
 * MAP METHODS *
 **************/
FirTest.prototype.drawPath = function(path, color)
{
  return new google.maps.Polyline({
    path: path,
    strokeColor: color,
    strokeOpacity: 0.5,
    strokeWeight: 4,
    map: this.map
  });
}

FirTest.prototype.drawPathFromJson = function()
{
  try
  {
    var json = jQuery.parseJSON( $("#taCoords").val() );

    // Empty buffer
    this.data.buffer = [];

    // Init values
    var bounds = new google.maps.LatLngBounds();
    var path = [];
    this.data.buffLat = [];
    this.data.buffLon = [];
    
    for(var i in json)
    {
      // Append to Google arrays
      var o = json[i];
      var ll = new google.maps.LatLng( o.latitude, o.longitude );
      path.push( ll );
      bounds.extend( ll );
	  
      // Add to buffer
      this.addToBuffer( this.data.buffLat, this.data.buffSize, parseFloat( o.latitude ) );
      this.addToBuffer( this.data.buffLon, this.data.buffSize, parseFloat( o.longitude ) );
    }
    
    // Clear previous paths
    this.clearPath([this.rawPath,this.firPath, this.regPath]);

    // Draw new path
    this.rawPath = this.drawPath(path, '#0000FF');
    
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
    var buffFirLat = [];
    var buffFirLon = [];
    var path = [];
    for(var i=0; i<json.length; i++)
    {
      var o = json[i];
      var lat = parseFloat(o.latitude);
      var lon = parseFloat(o.longitude);
      
      // Init/fill buffers from first values
      if(i==0)
      {
        buffFirLat = this.getFilledArray(this.data.coefs.length, lat);
        buffFirLon = this.getFilledArray(this.data.coefs.length, lon);
      }
      
      // Add to buffers
      this.addToBuffer( buffFirLat, this.data.firBuffSize, parseFloat( o.latitude ) );
      this.addToBuffer( buffFirLon, this.data.firBuffSize, parseFloat( o.longitude ) );
      
      // Get FIR filtered lat/lon values
      lat = this.getFirValue( lat, buffFirLat, this.data.coefs );
      lon = this.getFirValue( lon, buffFirLon, this.data.coefs );
      var ll = new google.maps.LatLng( lat, lon );
      
      path.push( ll );
    }		

    // Clear previous fir path
    this.clearPath( [this.firPath] );

    // Draw new path
    this.firPath = this.drawPath(path, '#FF0000');    
  }
  catch(e)
  {

    alert("Action failed");
    console.error(e.stack);
  }
};

FirTest.prototype.clearPath = function(paths)
{
  for(p in paths)
  {
    paths[p].setPath([]);
  }
};

FirTest.prototype.updateJson = function(event)
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
};

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

FirTest.prototype.getFirValue = function(val, oldVals, coefs)
{
  var result = 0, index = oldVals.length;
  for(var i=0; i<coefs.length; i++)
  {
    result += coefs[i] * oldVals[--index];
    if( index < 0 ) index = ( oldVals.length );
  }
  return result;
};

/****************
 * MISC METHODS *
 ***************/
FirTest.prototype.getFilledArray = function(len,val)
{
  var arr = new Array(len);
  while(--len >= 0) { arr[len] = val; }
  return arr;
};

FirTest.prototype.addToBuffer = function(buffer,bufferSize,val)
{
  // TODO: Check isValidPoint() first
  
	buffer.push( val );
	while( bufferSize > 0 && buffer.length > bufferSize ) 
  { 
    buffer.shift(); 
  }
};

/*****************************
 * HEADING, DISTANCE METHODS *
 ****************************/
FirTest.prototype.calcLinReg = function()
{
  // Determine direction based of width & height
  var e = this.getEdges();
  var w = this.getDistHaversine(e.minLat, e.minLon, e.minLat, e.maxLon);
  var h = this.getDistHaversine(e.minLat, e.minLon, e.maxLat, e.minLon);
  var h = 0;
  
  // Get linear regression heading in X-axis
  if( w > h )
  {
    var o = this.getLinearRegression(this.data.buffLon, this.data.buffLat);
    var path = [ new google.maps.LatLng(o.y1, o.x1), new google.maps.LatLng(o.y2, o.x2) ];
    h = parseInt( this.getBearing(o.y1,o.x1,o.y2,o.x2) );
  }
  // Get linear regression heading in Y-axis
  else
  {
    var o = this.getLinearRegression(this.data.buffLat, this.data.buffLon);
    var path = [ new google.maps.LatLng(o.x1, o.y1), new google.maps.LatLng(o.x2, o.y2) ];
    h = parseInt( this.getBearing(o.x1,o.y1,o.x2,o.y2) );
  }
  // Get distance
  var d = this.getDistHaversine(o.y1,o.x1,o.y2,o.x2);
  var dist = (d*1000).toFixed(1);
  
  // Draw
  this.regPath = this.drawPath(path, '#00FF00');
  
  // Set text
  $("#headistText").html( "Head: "+h+"&deg; Dist: "+ dist + " m");
}

FirTest.prototype.getEdges = function()
{
  var minLon = 181; var maxLon = -181;
  var minLat = 91; var maxLat = -91;
  for(i in this.data.buffLon)
  {
    var lon = this.data.buffLon[i];
    var lat = this.data.buffLat[i];
    minLon = Math.min(lon,minLon);
    minLat = Math.min(lat,minLat);
    maxLon = Math.max(lon,maxLon);
    maxLat = Math.max(lat,maxLat);
  }
  
  return { minLat: minLat, maxLat: maxLat, minLon: minLon, maxLon: maxLon }
};

FirTest.prototype.getBearing = function(lat1,lon1,lat2,lon2)
{
  lat1 = lat1.toRad(); lat2 = lat2.toRad();
  var dLon = (lon2-lon1).toRad();

  var y = Math.sin(dLon) * Math.cos(lat2);
  var x = Math.cos(lat1)*Math.sin(lat2) -
          Math.sin(lat1)*Math.cos(lat2)*Math.cos(dLon);
  return Math.atan2(y, x).toBrng();
}

FirTest.prototype.getDistHaversine = function(lat1,lon1,lat2,lon2)
{
  var R = 6371; // earth's mean radius in km
  var dLat = (lat2-lat1).toRad();
  var dLon = (lon2-lon1).toRad();
  lat1 = lat1.toRad(), lat2 = lat2.toRad();

  var a = Math.sin(dLat/2) * Math.sin(dLat/2) +
          Math.cos(lat1) * Math.cos(lat2) * 
          Math.sin(dLon/2) * Math.sin(dLon/2);
  var c = 2 * Math.atan2(Math.sqrt(a), Math.sqrt(1-a));
  var d = R * c;
  return d;
}

FirTest.prototype.getLinearRegression = function(values_x, values_y)
{
    // Init variables
    var sum_x = 0;
    var sum_y = 0;
    var sum_xy = 0;
    var sum_xx = 0;
    var count = 0;
    var x = 0;
    var y = 0;
    var values_length = values_x.length;

    // Calculate the sum for each of the parts necessary.
    for (var v = 0; v < values_length; v++) 
    {
        x = values_x[v];
        y = values_y[v];
        sum_x += x;
        sum_y += y;
        sum_xx += x*x;
        sum_xy += x*y;
        count++;
    }

    var b = (count*sum_xy - sum_x*sum_y) / (count*sum_xx - sum_x*sum_x);
    var a = (sum_y/count) - (b*sum_x)/count;
    
    // Build result
    var x1 = values_x[0];
    var y1 = x1 * b + a;
    var x2 = values_x[values_x.length-1];
    var y2 = x2 * b + a;
    
    return { x1: x1, y1: y1, x2: x2, y2: y2 }
}

/*******
* TODO *
*******/
FirTest.prototype.isValidPoint = function(oldPoint,newPoint)
{
  // TODO: Test if newPoint is within acceptable range compared to oldPoint
  // - If it is return true
  // - If not, add this.invalidCount++
  //   - If this.invalidCount < n return false
  //   - If global counet > n set n = 0 and return true
};

FirTest.prototype.getLatestPosition = function()
{
  // TODO: Return latest position from array
};

FirTest.prototype.getHeading = function()
{
  // TODO: 1. Check if standstill. If so, return 0
  // TODO: 2. Calculate linear regression angle
};

FirTest.prototype.getSpeed = function()
{
  // TODO: 1. Check if standstill. If so, return 0
  // TODO: 2. Calculate length of simple linear regression/time = speed
};