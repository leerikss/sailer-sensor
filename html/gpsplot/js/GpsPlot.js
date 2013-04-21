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
function GpsPlot() 
{
  // Init map 
  var options = 
    {
    mapTypeId: google.maps.MapTypeId.ROADMAP,
    center: new google.maps.LatLng(60.169845,24.938551), // <-- center to HELSINKI perkele!
    scaleControl: true,
    zoom: 18
    };


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

  this.map = new google.maps.Map( $("#map")[0], options );
  this.rawPath = new google.maps.Polyline([]);
  this.rawCs = [];
  this.hsdPath = new google.maps.Polyline([]);
  this.json = null;
  this.hsdFrom = -5;
};

/***************
 * MAP METHODS *
 **************/
GpsPlot.prototype.drawPath = function(path, color)
{
  return new google.maps.Polyline({
    path: path,
    strokeColor: color,
    strokeOpacity: 0.5,
    strokeWeight: 4,
    map: this.map
  });
}

GpsPlot.prototype.drawCircles= function(path, color)
{
  var cs = [];
  for(var i in path)
  {
    var opts =
    {
      map: this.map,
      strokeWeight: 0,
      fillColor: color,
      fillOpacity: 0.5,
      center: path[i],
      radius: 0.1
    };
    cs.push( new google.maps.Circle(opts) );
  }
  return cs;
}

GpsPlot.prototype.drawPathFromJson = function()
{
  try
  {
    this.json = jQuery.parseJSON( $("#taCoords").val() );

    var path = [];
    for(var i in this.json)
    {
      // Append to Google arrays
      var o = this.json[i];
      var ll = new google.maps.LatLng( o.latitude, o.longitude );
      path.push( ll );
    }
    
    // Clear previous paths
    this.clearPath([this.rawPath,this.rawCs,this.hsdPath]);

    // Draw new path
    this.rawPath = this.drawPath(path, '#0000FF');

    // Draw circles
    this.rawCs = this.drawCircles(path, '#000000');

    // Fit to map
    this.fitPath( path );
  }
  catch(e)
  {
    alert("Unable to parse JSON coords!");
    console.error(e.stack);
  }
};

GpsPlot.prototype.fitPath = function(path)
{
  var bounds = new google.maps.LatLngBounds();
  for(var i in path)
  {
    bounds.extend( path[i] );
  }

  if( !this.map.getBounds().contains( path[i] ) )
    this.map.panTo( path[i] );
//    this.map.fitBounds( bounds );
}

GpsPlot.prototype.hsdPrev = function()
{
  // Clear previous path
  this.clearPath([this.hsdPath]);

  // Get json
  if(this.json == null)
    this.json = jQuery.parseJSON( $("#taCoords").val() );      

  // Get from & step
  var step = parseInt( $("#buffer").val() );
  this.hsdFrom -= step;
  var to = this.hsdFrom + step;
  if(this.hsdFrom <= 0)
  {
    this.hsdFrom = 0;
    to = step;
  }

  // Build lats lons array
  var lats = [];
  var lons = [];
  for(var i = this.hsdFrom; i < to; i++)
  {
    lats.push( parseFloat( this.json[i].latitude ) );
    lons.push( parseFloat( this.json[i].longitude ) );
  }

  // Get data & output
  var data = this.getHsdData(lats,lons);
  this.hsdOut(data);
}

GpsPlot.prototype.hsdNext = function()
{
  // Clear previous path
  this.clearPath([this.hsdPath]);

  // Get json
  if(this.json == null)
    this.json = jQuery.parseJSON( $("#taCoords").val() );      

  // Get from & step
  var step = parseInt( $("#buffer").val() );
  this.hsdFrom += step;
  var to = this.hsdFrom + step;
  if( to >= this.json.length )
  {
    to = this.json.length;
    this.hsdFrom = to - step;
  }

  // Build lats lons array
  var lats = [];
  var lons = [];
  for(var i=this.hsdFrom; i<to; i++)
  {
    lats.push( parseFloat( this.json[i].latitude ) );
    lons.push( parseFloat( this.json[i].longitude ) );
  }

  // Get data & output
  var data = this.getHsdData(lats,lons);
  this.hsdOut(data);
}


GpsPlot.prototype.hsdOut = function(data)
{
  // Print heading,speed,distance
  $("#heading").html( data.heading + "&deg;" );
  // $("#speed").html( data.speed+" m" );
  $("#distance").html( data.distance );

  // Draw path
  this.hsdPath = this.drawPath(data.path, '#FF0000');

  // Fit to map
  this.fitPath( data.path );
}

GpsPlot.prototype.clearPath = function(paths)
{
  for(p in paths)
  {
    var p = paths[p];

    // Clear polygon paths
    if( p.setPath )
      p.setPath([]);

    // Clear circles
    else if( p.length )
    {
      for(var c in p)
      {
	p[c].setMap(null);
      }
    }

  }
};

GpsPlot.prototype.updateJson = function(event)
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

/*****************************
 * HEADING, DISTANCE METHODS *
 ****************************/
GpsPlot.prototype.getHsdData = function(lats,lons)
{
  // Determine direction based of width & height
  var e = this.getEdges(lats,lons);
  var w = this.getDistHaversine(e.minLat, e.minLon, e.minLat, e.maxLon);
  var h = this.getDistHaversine(e.minLat, e.minLon, e.maxLat, e.minLon);
  var hd = 0;

  // Get linear regression heading in X-axis
  if( w > h )
  {
    var o = this.getLinearRegression(lons, lats);
    var path = [ new google.maps.LatLng(o.y1, o.x1), new google.maps.LatLng(o.y2, o.x2) ];
    hd = parseInt( this.getBearing(o.y1,o.x1,o.y2,o.x2) );
  }
  // Get linear regression heading in Y-axis
  else
  {
    var o = this.getLinearRegression(lats, lons);
    var path = [ new google.maps.LatLng(o.x1, o.y1), new google.maps.LatLng(o.x2, o.y2) ];
    hd = parseInt( this.getBearing(o.x1,o.y1,o.x2,o.y2) );
  }
  // Get distance
  var d = this.getDistHaversine(o.y1,o.x1,o.y2,o.x2);
  var dist = (d*1000).toFixed(1);

  return { path: path, heading: hd, distance: dist }; 
}

  GpsPlot.prototype.getEdges = function(lats,lons)
{
  var minLon = 181; var maxLon = -181;
  var minLat = 91; var maxLat = -91;
  for(i in lons)
  {
    var lon = lons[i];
    var lat = lats[i];

    minLon = Math.min(lon,minLon);
    maxLon = Math.max(lon,maxLon);
    minLat = Math.min(lat,minLat);
    maxLat = Math.max(lat,maxLat);
  }
  
  return { minLat: minLat, maxLat: maxLat, minLon: minLon, maxLon: maxLon }
};

GpsPlot.prototype.getBearing = function(lat1,lon1,lat2,lon2)
{
  lat1 = lat1.toRad(); lat2 = lat2.toRad();
  var dLon = (lon2-lon1).toRad();

  var y = Math.sin(dLon) * Math.cos(lat2);
  var x = Math.cos(lat1)*Math.sin(lat2) -
          Math.sin(lat1)*Math.cos(lat2)*Math.cos(dLon);
  return Math.atan2(y, x).toBrng();
}

GpsPlot.prototype.getDistHaversine = function(lat1,lon1,lat2,lon2)
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

GpsPlot.prototype.getLinearRegression = function(values_x, values_y)
{
    // Init variables
    var sum_x = sum_y = sum_xy = sum_xx = sum_yy = count = x = y = 0;
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
        sum_yy += y*y;
        count++;

	console.log(x+","+y);

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
GpsPlot.prototype.isValidPoint = function(oldPoint,newPoint)
{
  // TODO: Test if newPoint is within acceptable range compared to oldPoint
  // - If it is return true
  // - If not, add this.invalidCount++
  //   - If this.invalidCount < n return false
  //   - If global counet > n set n = 0 and return true
};

GpsPlot.prototype.getLatestPosition = function()
{
  // TODO: Return latest position from array
};

GpsPlot.prototype.getHeading = function()
{
  // TODO: 1. Check if standstill. If so, return 0
  // TODO: 2. Calculate linear regression angle
};

GpsPlot.prototype.getSpeed = function()
{
  // TODO: 1. Check if standstill. If so, return 0
  // TODO: 2. Calculate length of simple linear regression/time = speed
};