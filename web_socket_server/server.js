const WebSocket = require("ws");

const WS_PORT = process.env.WS_PORT || 8888;

const wsServer = new WebSocket.Server({ port: WS_PORT }, () =>
  console.log(`WS server is listening at ws://localhost:${WS_PORT}`)
);

// array of connected websocket clients
let connectedClients = [];

wsServer.on("connection", (ws, req) => {
  console.log("Connected");
  // add new connected client
  connectedClients.push(ws);
  // listen for messages from the streamer, the clients will not send anything so we don't need to filter
  ws.on("message", (data) => {
    console.log(data);
    connectedClients.forEach((ws, i) => {
      if (ws.readyState === ws.OPEN) {
        ws.send(data);
      } else {
        connectedClients.splice(i, 1);
      }
    });
  });
});