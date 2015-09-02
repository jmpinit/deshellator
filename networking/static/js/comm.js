if (typeof define !== 'function') {
    var define = require('amdefine')(module);
}

define(function(require) {
    var exports = {};

    var socket = null;

    function sampleByte (src, x, y) {
        var i = y * src.width + x;
        var byte = 0;

        for (var bit = 0; bit < 8; bit++) {
            if (src.data[(i + bit) * 4] > 0) {
                byte |= 0x80 >> bit;
            }
        }

        return byte;
    }

    function serialize (src) {
        var w = src.width / 8;
        var h = src.height;
        var serialImage = [];

        for (var y = 0; y < h; y++) {
            for (var x = 0; x < w; x++) {
                serialImage[outIndex++] = sampleByte(src, x, y);
            }
        }

        return serialImage;
    }

    exports.connect = function (address) {
        socket = new WebSocket(address, 'screen-protocol');
    }

    exports.send = function (imageData) {
        if (socket && socket.readyState === WebSocket.OPEN) {
            socket.send(serialize(imageData));
        }
    }

    return exports;
});
