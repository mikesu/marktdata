// Listen for when the watchface is opened
Pebble.addEventListener('ready', 
  function(e) {
    console.log('PebbleKit JS ready!');
    Pebble.sendAppMessage({'JSReady': true},
    function(a){
    	console.log('send Success!');
    },function(error){
    	console.log('send false');
    });
  }
);

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
  function(e) {
    console.log('AppMessage received!');
 //    var dict = e.payload;
 //  	console.log('Got message: ' + JSON.stringify(dict));
 //  	if(dict['Code']) {
 //  		// The RequestData key is present, read the value
 //  		var value = dict['Code'];
 //  		Pebble.sendAppMessage({'Data': 'js_data'+value});
	// }
  }                     
);