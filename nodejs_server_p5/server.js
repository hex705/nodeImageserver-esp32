//i rewrote this to make it look familiar to me 
// it is based on the esp relay - i basically did a port // pirate of their code

// https://github.com/Inglebard/esp32cam-relay

// web server bits -- see shiffman 
const express = require('express');
const app = express();
app.use(express.static('public'));

// need in socket upgrade to distinguishing between "camera source" -- and "client viewers"
const url = require('url');

// add a ws web socket 
const WebSocket = require('ws'); // not socket io !

// launch teh server -- listening on port 3000
var server = app.listen(3000);
console.log("serverlistening on port 3000");

// set up two ws servers 
// may need a 3rd context for when esp32 with screen tries to get an image ? 
const wss1 = new WebSocket.Server({ noServer: true });
const wss2 = new WebSocket.Server({ noServer: true });
const wss3 = new WebSocket.Server({ noServer: true });

// no idea ... 
var cameraArray={};

// should add some sort of indicator that a connection has happened?



// 

//esp32cam websocket - receives camera source and relays to all browsers 
wss1.on('connection', function connection(ws) {
  console.log("got an image");
  ws.on('message', function incoming(message) {

    wss2.clients.forEach(function each(client) {
      console.log("sending to clients");
      if (client.readyState === WebSocket.OPEN) {
        client.send(message);
      }
    });

    wss3.clients.forEach(function each(client) {
      console.log("sending to screen clients");
  
    //  if (client.readyState === WebSocket.OPEN) {
        // client.send("hi"); // can huzzah get anything?
        client.send(message);
       //  client.send(binaryData);
    //  }
    });
    

  });


});

//webbrowser websocket - 
wss2.on('connection', function connection(ws) {
  ws.on('message', function incoming(message) {
  	// nothing here should be received
    console.log('received wss2: %s', message);
  });
});


// ok i stole this from the example ... and didn't touch it 
server.on('upgrade', function upgrade(request, socket, head) {
  const pathname = url.parse(request.url).pathname;
   
  if (pathname === '/jpgstream_tftClient') {
  console.log("got screen client");
   wss3.handleUpgrade(request, socket, head, function done(ws) {
      wss3.emit('connection', ws, request);
    });
   }

 else if (pathname === '/jpgstream_espCam') {
    wss1.handleUpgrade(request, socket, head, function done(ws) {
      wss1.emit('connection', ws, request);
    });

  } else if (pathname === '/jpgstream_webClient') {
    console.log("got a client " + socket);
    wss2.handleUpgrade(request, socket, head, function done(ws) {
      wss2.emit('connection', ws, request);
    });
  } else {
    socket.destroy();
  }
});


// this may not be needed ? i think it is tied to a framework i removed ? 
// app.get('/', (req, res) => {
//   	res.render('index', {});
// });


// server.listen(port, () => {
// 	  console.log(`App listening at http://localhost:${port}`)
// })

// experimental converting teh blob to arraybuffer???
// i think maybe the format of the data is killing teh connection - teh esp rejects and discsonnects

//https://stackoverflow.com/questions/50136991/how-can-i-send-binary-data-via-websocket-from-node-js-to-a-web-page

  // https://stackoverflow.com/questions/21797299/convert-base64-string-to-arraybuffer
    function base64ToArrayBuffer(base64) {
        var binary_string = Buffer.from(base64, 'base64').toString('binary');
        var len = binary_string.length;
        var bytes = new Uint8Array(len);
        for (var i = 0; i < len; i++) {
            bytes[i] = binary_string.charCodeAt(i);
        }    

        return bytes.buffer;
    }