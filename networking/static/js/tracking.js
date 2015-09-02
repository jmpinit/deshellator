if (typeof define !== 'function') {
    var define = require('amdefine')(module);
}

define(function(require) {
    var exports = {};

    var CV = require("cv");
    var POS = require("posit1");
    var SVD = require("svd");
    var AR = require("aruco");

    var video, canvas;
    var detector, posit;

    exports.setup = function (targetVideo) {
        video = targetVideo;
        var MODEL_SIZE = 35; // mm

        navigator.getUserMedia = navigator.getUserMedia || navigator.webkitGetUserMedia || navigator.mozGetUserMedia;
        navigator.getUserMedia({ video:true },
            function (stream) {
                if (window.URL) {
                    video.src = window.URL.createObjectURL(stream);
                } else if (video.mozSrcObject !== undefined) {
                    video.mozSrcObject = stream;
                } else {
                    video.src = stream;
                }
            },
            function(error){ }
        );

        canvas = document.createElement("canvas");
        canvas.width = video.width;
        canvas.height = video.height;

        detector = new AR.Detector();
        posit = new POS.Posit(MODEL_SIZE, 320);

        return canvas;
    }

    exports.getPose = function () {
        if (video.readyState === video.HAVE_ENOUGH_DATA) {
            var context = canvas.getContext('2d');
            context.drawImage(video, 0, 0, canvas.width, canvas.height);
            var image = context.getImageData(0, 0, canvas.width, canvas.height);
            var markers = detector.detect(image);

            if (markers.length > 0) {
                corners = markers[0].corners;

                for (i = 0; i < corners.length; ++ i){
                    corner = corners[i];

                    corner.x = corner.x - (canvas.width / 2);
                    corner.y = (canvas.height / 2) - corner.y;
                }

                return posit.pose(corners);
            } else {
                return null;
            }
        } else {
            return null;
        }
    }

    return exports;
});
