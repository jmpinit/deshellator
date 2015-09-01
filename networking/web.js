var WebSocketServer = require('websocket').server;

wsServer = new WebSocketServer({
    httpServer: server,
    autoAcceptConnections: false
});

var TIMEOUT = 5000; // ms
var lastFrame = null;
var lastTime = null;

wsServer.on('request', function(request) {
    var connection = request.accept('calc-protocol', request.origin);
    console.log((new Date()) + ' Connection accepted.');

    connection.on('message', function(message) {
        if (message.type === 'utf8') {
            console.log('Received Message: ' + message.utf8Data);
            connection.sendUTF(message.utf8Data);
        } else if (message.type === 'binary') {
            console.log('Received Binary Message of ' + message.binaryData.length + ' bytes');

            if (message.binaryData.length == 768) {
                var now = (new Date()).getTime();
                if (serialPort && serialPort.isOpen() && (lastTime === null || (now > lastTime + TIMEOUT))) {
                    serialPort.write(message.binaryData);
                    lastTime = now;
                }

                lastFrame = message.binaryData;

                var x = 0;
                var y = 0;

                var drawBuffer = "";

                for (var i = 0; i < message.binaryData.length; i++) {
                    for (var bit = 0; bit < 8; bit++) {
                        if (message.binaryData[i] & (0x80 >> bit)) {
                            drawBuffer += 'X';
                        } else {
                            drawBuffer += ' ';
                        }

                        x += 1;
                        if (x >= 96) {
                            x = 0;
                            y += 1;
                            drawBuffer += '\r\n';
                        }
                    }
                }

                process.stdout.write(drawBuffer);
            } else {
                console.log("weird length", message.binaryData.length);
            }
        }
    });

    connection.on('close', function(reasonCode, description) {
        console.log((new Date()) + ' Peer ' + connection.remoteAddress + ' disconnected.');
    });
});
