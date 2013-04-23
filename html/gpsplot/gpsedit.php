<?php
define("DATA", "data/");
?>
<!DOCTYPE html>
<html lang="en">
  <title>Gps Plot</title>
  <link rel="stylesheet" type="text/css" href="css/style.css">
  <link rel="stylesheet" type="text/css" href="css/gpsedit.css">

  <script type="text/javascript" src="js/jquery-1.8.3.min.js"></script>
  
  <script src="js/jquery.event.drag-2.2.js"></script>
  <script src="js/jquery.event.drag.live-2.2.js"></script>
  <script src="js/jquery.event.drop-2.2.js"></script>
  <script src="js/jquery.event.drop.live-2.2.js"></script>
  
  <script src="https://maps.googleapis.com/maps/api/js?AIzaSyDnUjPbtiGH9ZU8dxfOjJFBQC4Hv15TEWw&sensor=false"></script>
  <script type="text/javascript" src="js/GpsPlot.js"></script>

  <script type="text/javascript">

    // Init app
    $(document).ready(function() 
    {
      gpsPlot = new GpsPlot();
      
      // TESTING
      $( document ).drag("start",function( ev, dd ) {
			  return $('<div class="selection" />')
				.css('opacity', .65 )
				.appendTo( document.body );
		  })
		  .drag(function( ev, dd ) {
			  $( dd.proxy ).css({
				  top: Math.min( ev.pageY, dd.startY ),
				  left: Math.min( ev.pageX, dd.startX ),
				  height: Math.abs( ev.pageY - dd.startY ),
				  width: Math.abs( ev.pageX - dd.startX )
			  });
		  })
		  .drag("end",function( ev, dd ){
			  $( dd.proxy ).remove();
		  });
    
	    $('.drop').drop("start",function(){
			  $( this ).addClass("active");
		  })
		  .drop(function( ev, dd ){
			  $( this ).toggleClass("dropped");
		  })
		  .drop("end",function(){
			  $( this ).removeClass("active");
		  });
      
	    $.drop({ multi: true })      
	    
    });

  </script>

</head>

<body>

  <div id="forms">
  <div id="formspanel">

    <fieldset id="coords">
      <legend>GPS Data Files</legend>
<?php
if ($handle = opendir(DATA)) 
{
    $blacklist = array('.', '..', '*.php', '.DS_Store', 'gpx');
    while (false !== ($file = readdir($handle))) 
    {
        if (!in_array($file, $blacklist)) 
        {
            echo '<a class="left" href="#" onclick="javascript:gpsPlot.load(this, \''.DATA.$file.'\');">'.$file.'</a>';
            echo '<a class="right" href="#" onclick="javascript:gpsPlot.hide(\''.DATA.$file.'\');">Hide</a>';
            echo '<br class="clear"/>';
        }
    }
    closedir($handle);
}
?>      
    </fieldset>
    <div class="btns">
      <input type="button" value="Clean Up JSON" onclick="gpsPlot.cleanUpJson()"/>
      <input type="button" value="Reset" onclick="gpsPlot.reset()"/>
    </div>
    <br class="clear"/>

    <fieldset id="step">
      <legend>Step Data</legend>
        <span class="name">File:</span>
        <span class="value"id="file"></span>
        <br class="clear"/>
        <span class="name">Buffer:</span>
        <span class="value">
          <input type="text" class="input"id="stepBuff" value="5">
        </span>
        <br class="clear"/>
        <span class="name">Heading:</span>
        <span class="value"id="heading"></span>
        <br class="clear"/>
        <span class="name">Speed:<!--<br/>&nbsp;--></span>
        <span class="value"id="speed"></span>
        <br class="clear"/>
        <span class="name">Distance:</span>
        <span class="value"id="distance"></span>
        <br class="clear"/>
        <span class="name">Standstill:</span>
        <span class="value"id="standstill"></span>
        <br class="clear"/>
        <span class="name">Satellites:</span>
        <span class="value"id="sats"></span>
        <br class="clear"/>
        <span class="name">Epx:</span>
        <span class="value"id="epx"></span>
        <br class="clear"/>
        <span class="name">Epy:</span>
        <span class="value"id="epy"></span>
        <br class="clear"/>
    </fieldset>
    <div class="btns">
      <input type="button" value="Prev" id="prev" onclick="gpsPlot.step(-1);"/>
      <input type="button" value="Next" id="next" onclick="gpsPlot.step(1)"/>
    </div>
    <br class="clear"/>
    
    <fieldset id="fir">
      <legend>FIR Path</legend>
        <span class="name">Coefs:</span>
        <textarea rows="4" class="value" id="firCoefs">
0.25
0.25
0.25
0.25        
        </textarea>
    </fieldset>
	<div class="btns">
      <input type="button" value="Clear" onclick="gpsPlot.clearFirPath()"/>
      <input type="button" value="Draw" onclick="gpsPlot.drawFirPath()"/>
    </div>      
    
  </div> <!-- End formspanel -->
  </div> <!-- End forms -->

  <div id="map"></div>

  <div id="popup">  
    <fieldset>
      <legend>Cleaned Up JSON Data</legend>
      <textarea id="json"></textarea>
    </fieldset>
    <br/>
    <div class="right"><input id="close" type="button" value="Close"/>
  </div>
  
</body>
</html>