/*****************
*  EXTEND NUMBER *
*****************/
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
  var options = 
  {
    mapTypeId: google.maps.MapTypeId.ROADMAP,
    center: new google.maps.LatLng(60.169845,24.938551), // <-- center to HELSINKI perkele!
    scaleControl: true,
    zoom: 18
  };

  $(document).keypress(function(e){
  	console.log(e.which);
    if (e.which == 110){ $("#next").click(); }
    if (e.which == 112){ $("#prev").click(); }
  });

  this.map = new google.maps.Map( $("#map")[0], options );
  
  this.rawPath = new google.maps.Polyline([]);
  this.rawDots = [];
  this.linPath = new google.maps.Polyline([]);
  this.firPath = new google.maps.Polyline([]);
  
  this.json = null;
  this.start = -1;
  this.errCount = 0;
  
  this.STAND_STILL_DIST = 2;
  this.ERR_DIST_M = 10;
  this.ERR_MAX_COUNT = 3;
  this.MS_TO_KMH = 3.6;
  this.MS_TO_KNOT = 1.94384449244;
};

/**************************
 * METHODS CALLED FROM UI * 
 *************************/
GpsPlot.prototype.clearAll = function()
{
  this.clearGraphics([this.rawPath, this.rawDots, this.linPath, this.firPath]);
  this.start = -1;
};
 
GpsPlot.prototype.cleanUpJson = function()
{
  // Parse json
  var json = jQuery.parseJSON( $("#gps").val() );
    
  // Buil new json array
  var newJson = [];
  var prev_point = false;
  for(var i in json)
  {
    var curr_point = json[i];

    // First time set prevPoint to current point
    if(!prev_point)
      prev_point = curr_point;
        
    if( this.isValidPoint(curr_point, prev_point) )
    {
      // Only cache if a valid point
      newJson.push( curr_point );
      prev_point = curr_point;
    }
  }

  // Cache new json
  this.json = newJson;

  // Clear all previous paths
  this.clearAll();
    
  // Debug
  console.log( "Originl length: "+json.length );
  console.log( "New length: "+newJson.length );
    
  // Set new json data into textarea
  $("#gps").val( JSON.stringify(newJson, null, 2) );
};

GpsPlot.prototype.drawPathFromJson = function()
{
  // Parse json
  this.json = jQuery.parseJSON( $("#gps").val() );

  var path = [];
  for(var i in this.json)
  {
    // Append to Google arrays
    var o = this.json[i];
    var ll = new google.maps.LatLng( o.latitude, o.longitude );
    path.push( ll );
  }
    
  // Clear all
  this.clearAll();

  // Draw new path
  this.rawPath = this.drawPath(path, '#0000FF');

  // Draw circles
  this.rawDots = this.drawCircles(path, '#000000');

  // Fit to map
  this.fitPath( path );
};

GpsPlot.prototype.step = function(step)
{
  // Parse json
  this.json = jQuery.parseJSON( $("#gps").val() );      

  // Start & end indexes
  this.start += step;
  var steps = parseInt( $("#stepBuff").val() );
  var end = this.start + steps;
  if( this.start < 0 ) 
    this.start = 0;
  if( this.start > this.json.length-1-steps )
    this.start = this.json.length-1-steps;
  if( end < steps - 1 ) 
    end = steps-1;
  if( end > this.json.length-1 ) 
    end = this.json.length-1;
    
  // Get array slice as buffer
  var buff = this.json.slice(this.start, end);
  
  // Get linear regression line of buffer
  var line = this.getLinRegLine( buff );
  
  // Get time diff between first and last point in buffer
  var time = this.getTimeDiff( buff[buff.length-1].time , buff[0].time );
	
  // Get distance in meters
  var d = this.getDistHaversine( line.x1, line.y1, line.x2, line.y2 );
  var meters = (d * 1000);
  
  // Get speed in ms, kmh & knots
  var ms = ( meters / time ) * 1000;
  var kmh = ms * this.MS_TO_KMH;
  var knots = ms * this.MS_TO_KNOT;  
  
  // Get heading
  var heading = this.getBearing(line.x1, line.y1, line.x2, line.y2);
  
  // Output calculated data
  $("#heading").html( heading.toFixed(1) + "&deg;" );
  $("#speed").html( kmh.toFixed(1) + " km/h<br/>" + 
  	knots.toFixed(1)+" knots" );
  $("#distance").html( meters.toFixed(1) + " m" );
  
  // Output last point data
  var lastPoint = buff[buff.length - 1];
  $("#sats").html( lastPoint.satellites );
  $("#epx").html( lastPoint.epx );
  $("#epy").html( lastPoint.epy );

  // Clear previous path
  this.clearGraphics([this.linPath]);
  
  // Draw path
  var path = [ new google.maps.LatLng(line.x1, line.y1), 
  	new google.maps.LatLng(line.x2, line.y2) ];
  this.linPath = this.drawPath(path, '#FF0000', 1);

  // Fit path to map
  this.fitPath( path );
}

GpsPlot.prototype.drawFirPath = function()
{
  // Parse json
  if(!this.json)
    this.json = jQuery.parseJSON( $("#gps").val() ); 
    
  // Get FIR filtered values
  var firVals = this.getFirArray(this.json);

  // Build path  
  var path = [];
  for(var i in firVals)
  {
    var o = firVals[i];
    var lat = parseFloat(o.latitude);
    var lon = parseFloat(o.longitude);
    var ll = new google.maps.LatLng( lat, lon );
    path.push( ll );
  }		

  // Clear previous fir path
  this.clearGraphics( [this.firPath] );

  // Draw new path
  this.firPath = this.drawPath(path, '#00FF00');  

  // Fit path to map
  this.fitPath( path );  
}

GpsPlot.prototype.clearGraphics = function(g)
{
  for(p in g)
  {
    var p = g[p];

    // Clear polygon paths
    if( p.setPath ) p.setPath([]);

    // Clear circles
    else if( p.length )
    {
      for(var c in p) {  p[c].setMap(null); }
    }
  }
};

 /**********************
 * MAP DRAWING METHODS *
 **********************/
GpsPlot.prototype.drawPath = function(path, color, opa)
{
  opa = (!opa) ? 0.5 : opa;
  
  return new google.maps.Polyline({
    path: path,
    strokeColor: color,
    strokeOpacity: opa,
    strokeWeight: 4,
    map: this.map
  });
}

GpsPlot.prototype.drawCircles = function(path, color)
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
      radius: 0.5
    };
    cs.push( new google.maps.Circle(opts) );
  }
  return cs;
}

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

/*******************
* FIR METHODS *
*******************/
GpsPlot.prototype.getFilledArray = function(len,val)
{
  var arr = new Array(len);
  while(--len >= 0) { arr[len] = val; }
  return arr;
};

GpsPlot.prototype.parseFirCoefs = function()
{
  var txt = $("#firCoefs").val();
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

GpsPlot.prototype.addToBuffer = function(buffer,bufferSize,val)
{
	buffer.push( val );
	while( bufferSize > 0 && buffer.length > bufferSize ) 
  { 
    buffer.shift(); 
  }
};

GpsPlot.prototype.getFirArray = function(buff)
{
  // Parse coefs
  var coefs = this.parseFirCoefs();

  var buffFirLat = []; var buffFirLon = []; var result = [];
  
  for(var i=0; i<buff.length; i++)
  {
    var o = buff[i];
    var lat = parseFloat(o.latitude);
    var lon = parseFloat(o.longitude);
      
    // First round init buffers
    if(i==0)
    {
      buffFirLat = this.getFilledArray(coefs.length, lat);
      buffFirLon = this.getFilledArray(coefs.length, lon);
    }
      
    // Add to buffers latest values
    this.addToBuffer( buffFirLat, coefs.length, lat );
    this.addToBuffer( buffFirLon, coefs.length, lon );
      
    // Get FIR filtered lat/lon values
    var firLat = this.getFirValue( lat, buffFirLat, coefs );
    var firLon = this.getFirValue( lon, buffFirLon, coefs );
      
    result.push( { latitude: firLat, longitude: firLon } );
  }
  
  return result;
}

GpsPlot.prototype.getFirValue = function(val, oldVals, coefs)
{
  var result = 0, index = oldVals.length;
  for(var i=0; i<coefs.length; i++)
  {
    result += coefs[i] * oldVals[--index];
    if( index < 0 ) index = ( oldVals.length );
  }
  return result;
};

/***************************
 * STEP DATA MATH METHODS *
 **************************/
 GpsPlot.prototype.getTimeDiff = function(t1, t2)
{
  var time1 = Date.parse( t1 );
  var time2 = Date.parse( t2 );
  return ( time1 - time2 );
}

GpsPlot.prototype.isValidPoint = function(curr_point, prev_point)
{
  // Get distance between points in meters
  var dist = this.getDistHaversine( parseFloat(curr_point.latitude), 
    parseFloat(curr_point.longitude), parseFloat(prev_point.latitude), 
    parseFloat(prev_point.longitude) ) * 1000;

  // Is in standstill
  // TODO: Maybe compare to average of buffered n points?
  if( dist <= this.STAND_STILL_DIST )
  {
    // TODO: Mark that we're in standstill
    console.log("Found a standstill point");
    return false;
  }
  
  // Found an unrealistic jump within
  if( dist >= this.ERR_DIST_M )
  {
    console.log("Found an errorous peak point");
    
    this.errCount++;
    
    if( this.errCount > this.ERR_MAX_COUNT )
    {
      console.log("Found too many continuous error peaks. Nomore assuming they're errors");
      this.errCount = 0;
      return true;
    }
    else
      return false;
  }
  // Remember to decrease count of continuous errors
  else if( this.errCount > 0 )
    this.errCount--;
      
  return true;
}

GpsPlot.prototype.getNewArrayByProp = function(arr,prop)
{
  var newArr = [];
  for(var i in arr)
  {
    newArr.push( parseFloat( arr[i][prop] ) );
  }
  return newArr;
}

GpsPlot.prototype.getLinRegLine = function(buff)
{
  // Determine direction based of width & height
  var e = this.getEdges(buff);
  var w = this.getDistHaversine(e.minLat, e.minLon, e.minLat, e.maxLon);
  var h = this.getDistHaversine(e.minLat, e.minLon, e.maxLat, e.minLon);
  
  var lats = this.getNewArrayByProp(buff, "latitude");
  var lons = this.getNewArrayByProp(buff, "longitude");
  
  var hd = 0;
  
  // Get linear regression heading in X-axis
  if( w > h )
  {
    var o = this.getLinearRegression(lons, lats);
    return { x1: o.y1, y1: o.x1, x2: o.y2, y2: o.x2 }
  }
  // Get linear regression heading in Y-axis
  else
  {
    var o = this.getLinearRegression(lats, lons);
    return { x1: o.x1, y1: o.y1, x2: o.x2, y2: o.y2 }
  }
  
};

GpsPlot.prototype.getEdges = function(buff)
{
  var minLon = 181; var maxLon = -181;
  var minLat = 91; var maxLat = -91;
  for(i in buff)
  {
    var lat = parseFloat( buff[i].latitude );
    var lon = parseFloat( buff[i].longitude );

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