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
  // Init Goolge Map
  var options = 
  {
    mapTypeId: google.maps.MapTypeId.ROADMAP,
    center: new google.maps.LatLng(60.169845,24.938551), // <-- center to HELSINKI perkele!
    scaleControl: true,
    zoom: 18
  };
  this.map = new google.maps.Map( $("#map")[0], options );

  // Can use "P" and "N" to step next/previous
  $(document).keypress(function(e){
    if (e.which == 110){ $("#next").click(); }
    if (e.which == 112){ $("#prev").click(); }
  });
  
  // Holder of currently loaded json
  this.json = null;
  
  // Colors & paths
  this.RAW_COLS = ['#0000FF','#8904B1'];
  this.rawCol = [];
  this.colIndex = 0;
  this.FIR_COL = '#00FF00';
  this.STEP_COL = '#FF0000';
  this.paths = [];

  // Step calculations  
  this.stepIndex = [];
  this.errCount = 0;
  this.REC_MIN_SECS = 1;
  this.REC_MAX_DIST_M = 50;
  this.ERR_MAX_COUNT = 3;
  this.STAND_STILL_M = 3;
  this.MS_TO_KMH = 3.6;
  this.MS_TO_KNOT = 1.94384449244;
};

/**************************
 * METHODS CALLED FROM UI * 
 *************************/
GpsPlot.prototype.initDates = function()
{
  var url = '/getDates';
  $.getJSON(url, function(data) {
      for(var item in data)
      {
	var itm = data[item];
	var date = itm.date;
	var dd = date.substring(0,2);
	var mm = date.substring(3,5);
	var yyyy = date.substring(6);
	var url = '/getDataByDate?date='+yyyy+'-'+mm+'-'+dd;
	var row = '<a class="left" href="#" onclick="javascript:gpsPlot.load(this, \''+url+'\');">'+itm.date+'</a>';
	row += '<a class="right" href="#" onclick="javascript:gpsPlot.hide(\''+url+'\');">Hide</a>';
	row += '<br class="clear"/>';
	$("#dates").append(row);
      }
    });    
}

GpsPlot.prototype.load = function(n, url)
{
  var _this = this;
  $.getJSON(url, function(data) {
  
    // Init
    _this.json = data;
    _this.id = url;
    $("#file").html( n.innerText );
    
    // Don't redraw if already drawn
    if( _this.paths['raw_'+url] )
      return;

    // Set color
    var col = _this.setRawCol( _this.id);
    
    // Draw raw path
    _this.drawRaw(data);

    // Reset all previous steps
    _this.resetSteps();
    _this.stepIndex[_this.id] = -1;
  });
}

GpsPlot.prototype.hide = function(id)
{
  this.deletePath('raw_'+id);
  this.deletePath('dot_'+id);
};

GpsPlot.prototype.reset = function()
{
  // Delete all paths
  for(var id in this.paths) 
  { 
    this.deletePath(id); 
  }
  this.resetSteps();
  $("#file").html( "" );
  this.json = false;
};

GpsPlot.prototype.cleanUpJson = function()
{
  if(!this.json)
  {
    alert("No JSON loaded");
    return;
  }
  
  // Build new json array
  var newJson = [];
  var prev_point = false;
  for(var i in this.json)
  {
    var curr_point = this.json[i];

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

  // Debug
  console.log( "Originl length: "+this.json.length );
  console.log( "New length: "+newJson.length );

  // Set new json
  this.json = newJson;

  // Reset step
  this.resetSteps();
  $("#file").html( "" );
  
  // Redraw
  this.drawRaw();
  
  // Popup & add text
  $("#json").val( JSON.stringify(newJson, undefined, 2) );
  $("#popup").css({
    left: ( $(window).width()/2 - $('#popup').width()/2 ),
    top: ( $(window).height()/2 - $('#popup').height()/2 ),
  });
  $("#popup").fadeIn(500);
  $("#close").click(function(){
    $("#popup").fadeOut(500);
  });
};

GpsPlot.prototype.step = function(step)
{
  if(!this.json)
  {
    alert("No JSON loaded");
    return;
  }

  // Start & end indexes
  this.stepIndex[this.id] += step;
  var steps = parseInt( $("#stepBuff").val() );
  var end = this.stepIndex[this.id] + steps;
  if( this.stepIndex[this.id] < 0 ) 
    this.stepIndex[this.id] = 0;
  if( this.stepIndex[this.id] > this.json.length-1-steps )
    this.stepIndex[this.id] = this.json.length-1-steps;
  if( end < steps - 1 ) 
    end = steps-1;
  if( end > this.json.length-1 ) 
    end = this.json.length-1;
    
  // Get array slice as buffer
  var buff = this.json.slice(this.stepIndex[this.id], end);
  
  // Get linear regression line of buffer
  var e = this.getEdges(buff);
  var lats = this.getNewArrayByProp(buff, "latitude");
  var lons = this.getNewArrayByProp(buff, "longitude");
  var line = this.getLinRegLine( e, lats, lons );
  
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
  
  // Get standstill
  var standstill = this.isStandStill(e);
  
  // Output calculated data
  $("#heading").html( heading.toFixed(1) + "&deg;" );
  // $("#speed").html( kmh.toFixed(1)+" km/h" ); // knots.toFixed(1)+" knots" );
  $("#speed").html( knots.toFixed(1)+" knots" );
  $("#distance").html( meters.toFixed(1) + " m" );
  $("#standstill").html( standstill );
  
  // Output last point data
  var lastPoint = buff[buff.length - 1];
  $("#sats").html( lastPoint.satellites );
  $("#epx").html( lastPoint.epx );
  $("#epy").html( lastPoint.epy );

  // Construct & draw path
  var path = [ new google.maps.LatLng(line.x1, line.y1), 
  	new google.maps.LatLng(line.x2, line.y2) ];
  this.drawPath('step', path, this.STEP_COL, 1);

  // Fit path to map
  // this.fitPath( path );
}

GpsPlot.prototype.drawFirPath = function()
{
  if(!this.json)
  {
    alert("No JSON loaded");
    return;
  }    
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

  // Draw fir path
  this.drawPath('fir', path, this.FIR_COL);  

  // Fit path to map
  this.fitPath( path );  
}

GpsPlot.prototype.clearFirPath = function()
{
  this.paths['fir'].setPath([]);
}

 /**********************
 * MAP DRAWING METHODS *
 **********************/
GpsPlot.prototype.setRawCol = function(id)
{
  if( this.rawCol[id] )
    return;
  this.rawCol[id] = this.RAW_COLS[this.colIndex];
  this.colIndex++;
  if( this.colIndex >= this.RAW_COLS.length )
    this.colIndex = 0;
  return this.rawCol[id];
}

GpsPlot.prototype.drawRaw = function()
{
  // Append to Google path array
  var path = [];
  for(var i in this.json)
  {
    var o = this.json[i];
    var ll = new google.maps.LatLng( parseFloat(o.latitude), parseFloat(o.longitude) );
    path.push( ll );
  }
    
  // Draw path + dots
  this.drawPath('raw_'+this.id, path, this.rawCol[this.id] );
  this.drawDots('dot_'+this.id, path, this.rawCol[this.id] );

  // Fit to map
  this.fitPath( path );
};

GpsPlot.prototype.deletePath = function(id)
{
  var o = this.paths[id];
  if(!o) return;
  
  // Clear polygon
  if(o.setPath) o.setPath([]);
    
  // Clear dots
  else if(o.length )
  {
    for(var d in o) {  o[d].setMap(null); }
  }
  
  this.paths[id] = false;
}

GpsPlot.prototype.drawPath = function(id, path, color, opa)
{
  // Clear previous path
  if( this.paths[id] )
    this.paths[id].setPath([]);

  // Was opacity passed
  opa = (!opa) ? 0.5 : opa;
  
  // Draw & cache path
  this.paths[id] = new google.maps.Polyline({
    path: path,
    strokeColor: color,
    strokeOpacity: 1,
    strokeWeight: 4,
    map: this.map
  });
}

GpsPlot.prototype.drawDots = function(id, path, color)
{
  // Clear previous dots
  if( this.paths[id] )
  {
    for(var d in this.paths[id]) {  this.paths[id][d].setMap(null); }
  }

  // Draw & cache dots
  var cs = [];
  for(var i in path)
  {
    var opts =
    {
      map: this.map,
      strokeWeight: 0,
      fillColor: color,
      fillOpacity: 1,
      center: path[i],
      radius: 1
    };
    cs.push( new google.maps.Circle(opts) );
  }
  
  this.paths[id] = cs;
}

GpsPlot.prototype.fitPath = function(path)
{
  var bounds = new google.maps.LatLngBounds();
  for(var i in path)
  {
    bounds.extend( path[i] );
  }

  //if( !this.map.getBounds().contains( path[0] ) )
  //  this.map.panTo( path[0] );
  
  this.map.fitBounds( bounds );
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
GpsPlot.prototype.resetSteps = function()
{
  for(var i in this.stepIndex)
  {
    this.stepIndex[i] = -1;
  }
}

GpsPlot.prototype.getTimeDiff = function(t1, t2)
{
  var time1 = (typeof(t1) == "string") ? Date.parse( t1 ) : new Date(t1 * 1000);
  var time2 = (typeof(t2) == "string") ? Date.parse( t2 ) : new Date(t2 * 1000);
  
  return ( time1 - time2 );
}

GpsPlot.prototype.isValidPoint = function(curr_point, prev_point)
{
  // Get distance between points in meters
  var dist = this.getDistHaversine( parseFloat(curr_point.latitude), 
    parseFloat(curr_point.longitude), parseFloat(prev_point.latitude), 
    parseFloat(prev_point.longitude) ) * 1000;

  // Get time difference between two points
  var time = this.getTimeDiff( curr_point.time , prev_point.time ) / 1000;
  
  /*
  if( time < this.REC_MIN_SECS )
  {
    console.log("Skipping point: Time difference < "+this.REC_MIN_SECS+" secs");
    return false;
  }	
  */
  
  // Found an unrealistic jump
  if( dist >= this.REC_MAX_DIST_M ) // && time < this.REC_MAX_DIST_S )
  {
    console.log("Skipping point: Distance > "+this.REC_MAX_DIST_M+" m");
    
    this.errCount++;
    
    if( this.errCount > this.ERR_MAX_COUNT )
    {
      console.log("Found too many distance errors. No more assuming they're errors");
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

GpsPlot.prototype.isStandStill = function(e)
{
  var w = this.getDistHaversine(e.minLat, e.minLon, e.minLat, e.maxLon) * 1000;
  var h = this.getDistHaversine(e.minLat, e.minLon, e.maxLat, e.minLon) * 1000;
  
  if(w < this.STAND_STILL_M && h < this.STAND_STILL_M )
  	return "<b>true</b>";
  	
  return "false";  
};

GpsPlot.prototype.getNewArrayByProp = function(arr,prop)
{
  var newArr = [];
  for(var i in arr)
  {
    newArr.push( parseFloat( arr[i][prop] ) );
  }
  return newArr;
}

GpsPlot.prototype.getLinRegLine = function(e, lats, lons)
{
  // Determine direction based of width & height
  var w = this.getDistHaversine(e.minLat, e.minLon, e.minLat, e.maxLon);
  var h = this.getDistHaversine(e.minLat, e.minLon, e.maxLat, e.minLon);
  
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
