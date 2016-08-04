// Listen for when the watchface is opened
Pebble.addEventListener('ready', 
  function(e) {
    console.log('PebbleKit JS ready!');
    Pebble.sendAppMessage({"init":"js reday"}, 
      function(e) {
        console.log('sent js reday');
      },function(e) {
        console.log('Failed to send data length to Pebble!');
      }
    );
  }
);

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
  function(e) {
    var dict = e.payload;
  	console.log('Got message: ' + JSON.stringify(dict));
  	if(dict.action) {
      if("get_list" == dict.action){
        queryList(dict.data);
      }else if("get_detail" == dict.action){
        queryDetail(dict.data);
      }else if("init" == dict.action){
        Pebble.sendAppMessage({"init":"js reday"}, 
          function(e) {
            console.log('sent c reday');
          },function(e) {
            console.log('Failed to send data length to Pebble!');
          }
        );
      }
    }
  }                     
);

var MAX_CHUNK_SIZE = 4000;  // From app_message_inbox_size_maximum()
var MAX_TRY = 3;

function queryList(codes){
  queryItems(codes,function(items){
    sendItem(items,0);
  });
}

function sendItem(items,index){
  console.log('Send items with index ' + index);
  var item = items[index];
  if(index == 0){
    item.list_size = items.length;
  }
  Pebble.sendAppMessage(item, 
    function(e) {
      console.log('Success to sent items with index!' + index);
      if(index+1<items.length){
        sendItem(items,index+1);
      }
    },function(e) {
      console.log('Failed to send items with index!' + index);
    }
  );
}

function queryDetail(code){
  queryItems(code,function(items){
    downloadImage(code,function(image){
      sendDetail(items[0],image,0);
    })
  });
}

function sendDetail(item,image,index){
  console.log('Send detail');
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

  dict.img_chunk = image.slice(index, index + chunkSize);
  dict.img_chunk_size = chunkSize;
  dict.img_chunk_index = index;

  // Send the chunk
  Pebble.sendAppMessage(dict, function() {
    console.log('Success to Sent chunk with index:' + index + ' chunkSize:'+chunkSize);
    // Success
    index += chunkSize;
    if(index < imageSize) {
      // Send the next chunk
      sendDetail(item,image, index);
    }
  }, function(e) {
    console.log('Failed to send chunk with index:' + index);
  });
}


function queryItems(codes,callback){
  var codesArray = codes.split(",");
  var codesForQuery = "";
  for (var i = 0; i < codesArray.length; i++) {
    codesForQuery = codesForQuery + ",s_" +codesArray[i];
  }
  if(codesForQuery.length>0){
    codesForQuery = codesForQuery.slice(1);
  }
  var queryUrl = "http://hq.sinajs.cn/list="+codesForQuery;
  var request = new XMLHttpRequest();
  request.open("GET", queryUrl,true);
  request.onload = function(e) {
    console.log("queryItems onload");
    var result = request.response;
    if(request.status==200&&result!=null&&result.length>0){
      var items = [];
      var lines = result.split("\n");
      for(i = 0; i < lines.length; i++) {
        var line = lines[i];
        if(line.length>3){
          var itemStr = line.slice(line.indexOf("\"")+1,line.lastIndexOf("\""));
          var itemProperties = itemStr.split(",");
          var item = {};
          item.item_code = codesArray[i];
          item.item_name = itemProperties[0];
          item.item_value = itemProperties[1];
          item.item_point = itemProperties[2];
          item.item_rate = itemProperties[3]+"%";
          item.item_volume = itemProperties[4];
          item.item_turnover = itemProperties[5];
          items.push(item);
        }
      }
      callback(items);
    }else{
       console.log('queryItems:' + codes +" Failed !");
    } 
  }
  request.onerror = function(e) {
      console.log("queryItems Error " + e);
  }
  request.send(null);
}

function downloadImage(code,callback) {
  var imgUrl = "http://image.sinajs.cn/newchart/wap/cn_min_other/min_wap/"+code+".png";
  var request = new XMLHttpRequest();
  request.responseType = "arraybuffer";
  request.open("GET", imgUrl,true);
  request.onload = function(e) {
    console.log("downloadImage onload");
    var buf = request.response;
    if(request.status == 200 && buf) {
        var byteArray = new Uint8Array(buf);
        var arr = [];
        for(var i=0; i<byteArray.byteLength; i++) {
            arr.push(byteArray[i]);
        }

        console.log("Downloaded file with " + byteArray.length + " bytes.");
        callback(arr);
    }
    else {
      errorCallback("Request status is " + request.status);
    }
  }
  request.onerror = function(e) {
      console.log("downloadImage Error " + e);
  }
  request.send(null);
}

