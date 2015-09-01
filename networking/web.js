var util = require('util'),
    EventEmitter = require('events').EventEmitter,
    WebSocketServer = require('websocket').server;

var exports = {};

var FRAME_WIDTH = 96;
var FRAME_HEIGHT = 64;
var FRAME_BYTES = FRAME_WIDTH * FRAME_HEIGHT / 8;

exports.App = function (httpServer) {
    this.currentFrame = new Buffer(FRAME_BYTES);

    this.server = new WebSocketServer({
        httpServer: httpServer,
        autoAcceptConnections: false
    });

    this.server.on('request', (function(request) {
        var connection = request.accept('screen-protocol', request.origin);
        console.log((new Date()) + ' Connection accepted.');

        connection.on('message', (function(message) {
            if (message.type === 'utf8') {
                this.emit("message", message);
            } else if (message.type === 'binary') {
                console.log('Received Binary Message of ' + message.binaryData.length + ' bytes');

                if (message.binaryData.length == FRAME_BYTES) {
                    this.currentFrame = message.binaryData;
                }
            }
        }).bind(this));

        connection.on('close', (function(reasonCode, description) {
            console.log((new Date()) + ' Peer ' + connection.remoteAddress + ' disconnected.');
            this.emit("close");
        }).bind(this));
    }).bind(this));
}

util.inherits(exports.App, EventEmitter);

module.exports = exports;
