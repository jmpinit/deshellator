if (typeof define !== 'function') {
    var define = require('amdefine')(module);
}

define(function(require) {
    var exports = {};

    function orderedDither (src) {
        var w = src.width;
        var h = src.height;

        var bayerMatrix = [
            [1, 9, 3, 11],
            [13, 5, 15, 7],
            [4, 12, 2, 10],
            [16, 8, 14, 6],
        ]

        var dithered = new ImageData(new Uint8ClampedArray(4 * w * h), w, h);

        for (var y = 0; y < h; y++) {
            for (var x = 0; x < w; x++) {
                var isrc = (y * w + x) * 4;
                var idest = ((h - 1 - y) * w + x) * 4;

                var brightness = (src[isrc] + src[isrc + 1] + src[isrc + 2]) / 3.0;
                var limit = 255.0 * bayerMatrix[y % 4][x % 4] / 17.0;

                if (brightness < limit) {
                    dithered.data[idest] = 0;
                    dithered.data[idest + 1] = 0;
                    dithered.data[idest + 2] = 0;
                } else {
                    dithered.data[idest] = 255;
                    dithered.data[idest + 1] = 255;
                    dithered.data[idest + 2] = 255;
                }

                // alpha
                dithered.data[idest + 3] = 255;
            }
        }

        return dithered;
    }

    exports.render = function (imageData) {
        var w = imageData.width;
        var h = imageData.height;

        return orderedDither(imageData);
    }

    return exports;
});
