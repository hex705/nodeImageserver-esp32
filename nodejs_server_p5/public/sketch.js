// so for some reason you don't have to import ws sockets in the index.html file ??? 

// i rewrote this example to make it look familiar to me 
// it is based on the esp relay - i basically did a port // pirate of their code

// https://github.com/Inglebard/esp32cam-relay

let img;
let ws;

let host = '127.0.0.1'; // maybe should be local host ?
let port = 3000;
let path = "/jpgstream_webClient"; // the server checks what kind of connection you are 

function setup() {
  // Create the canvas using p5.js
  createCanvas(640, 480); // Default size; will resize based on image later
   imageMode(CENTER);
  //By default, rotations are specified in radians
  angleMode(DEGREES);

  // Build WebSocket URL -- this is a dynamic web url builder -- it's a bit of overkill 
  // it checks if the window is http or https then sets socket to correct prpotocol 
  // it is extracting the port -- but we could assign it 
  // it then assembles the url for teh scoket 
  let webProtocolStr = location.protocol === 'https:' ? 'wss' : 'ws'; // location is of window - its DOM
  let portStr = location.port ? ':' + location.port : '';
 // let wsUrl = webProtocolStr + '://' + location.hostname + portStr + "/jpgstream_client";
 let   wsUrl = ("ws://"+host+":"+port+path);

  console.log(wsUrl);

  // Open WebSocket connection
  ws = new WebSocket(wsUrl);
  ws.onopen = socketConnected;
  ws.onmessage = onMessageReceived; // set up event handler for message arrival (onmessage is builtin, but what happens once it arrives can be defined)
}

// trying this - onopen for socket
function socketConnected(messageEvent){
  // not sure you can see any event data -- check shifman vid -- may only be io?
  console.log("client socket connected " + messageEvent.data +"\n"+ messageEvent);

}

// message arrival event handler
function onMessageReceived (message){
  // this needed to be found in examples and translated
   let blob = message.data; // the binary data from the esp
   let url = URL.createObjectURL(blob); // a url to access teh binary data 
  // img = loadImage(url, onImageLoaded);
     img = loadImage(url);
}

// so this used to do more stuff -- now its all in last line of function above 

// function onImageLoaded(loadedImg){
//   img = loadedImg;
//  // resizeCanvas(img.width, img.height);
//  // redraw();
// }


function draw() {
  if (img) {
    push();
      translate(img.width/2,img.height/2);
      rotate(-90);
      image(img, 0, 0, img.width, img.height);
    pop();
  }
}
