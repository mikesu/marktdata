// Listen for when the watchface is opened
Pebble.addEventListener('ready', 
  function(e) {
    console.log('PebbleKit JS ready!');
  }
);

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
  function(e) {
    var dict = e.payload;
  	console.log('Got message: ' + JSON.stringify(dict));
  	if(dict['action']) {
  		var action = dict['action'];
      if("get_list" == action){
        var codes = dict['data'];
        queryList(codes);
      }eles if("get_detail" = action){
        var code = dict['data'];
        queryDetail(code);
      }
    }
  }                     
);

var MAX_CHUNK_SIZE = 8000;  // From app_message_inbox_size_maximum()

function queryList(codes){
  var items = queryItems(codes);
  sendItem(items,0);
}

function sendItem(items,index){
  var item = items[index];
  if(index == 0){
    item.list_size = items.length;
  }
  Pebble.sendAppMessage(item, 
    function(e) {
      if(index+1<items.length){
        sendItems(items,index+1);
      }
    },function(e) {
      console.log('Failed to send data length to Pebble!');
    }
  );
}

function queryDetail(code){
  var items = queryItems(code);
  var image = downloadImage(code);
  sendDetail(items[0],image,0);
}

function sendDetail(item,image,index){
  var imageSize = image.length;
  var dict;
  if(index==0){
    dict = item;
    dict.img_size = imageSize;
  }else{
    dict = {};
  }

 // Determine the next chunk size
  var chunkSize;
  if(imageSize - index < MAX_CHUNK_SIZE) {
    // Will only need one more chunk
    chunkSize = imageSize - index;
  } else {
    // Will require multiple chunks for remaining data
    chunkSize = MAX_CHUNK_SIZE;
  }

  dict.img_chunk = array.slice(index, index + chunkSize);
  dict.img_chunk_size = chunkSize;
  dict.img_chunk_index = index;

  // Send the chunk
  Pebble.sendAppMessage(dict, function() {
    // Success
    index += chunkSize;

    if(index < imageSize) {
      // Send the next chunk
      sendDetail(item,image, index);
    }
  }, function(e) {
    console.log('Failed to send chunk with index ' + index);
  });
}


function queryItems(codes){
  var queryUrl = "http://hq.sinajs.cn/list=s_"+codes;
  var request = new XMLHttpRequest();
  request.open("GET", queryUrl,false);
  request.send();
  if(request.status==200){
    var items = [];
    var lines = request.response.split("\n");
    for(var i = 0; i < lines.length; i++) {
      var line = lines[i];
      var itemStr = line.substr(line.indexof("\"")+1,line.lastIndexOf("\""));
      var itemProperties = itemStr.split(",");
      var item = {};
      item.code = itemProperties[0];
      item.name = itemProperties[0];
      item.value = itemProperties[1];
      item.point = itemProperties[2];
      item.rate = itemProperties[3];
      item.volume = itemProperties[4];
      item.turnover = itemProperties[5];
      items.push(item);
    }
    return items;
  }else{
    return null;
  } 
}

function downloadImage(code) {
  var imgUrl = "http://image.sinajs.cn/newchart/wap/cn_min_other/min_wap/"+code+".png";
  var request = new XMLHttpRequest();
  request.responseType = "arraybuffer";
  request.open("GET", imgUrl,false);
  request.send();
  if(request.status==200){
    var byteArray = new Uint8Array(request.response);
    var array = [];
    for(var i = 0; i < byteArray.byteLength; i++) {
      array.push(byteArray[i]);
    }
    return array;
  }else{
    return null;
  }
}

