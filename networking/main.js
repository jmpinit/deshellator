var util = require("util"),
    http = require('http'),
    finalhandler = require('finalhandler'),
    serveStatic = require('serve-static'),
    EventEmitter = require('events').EventEmitter;

var hardware = require('./hardware'),
    web = require('./web');

var serve = serveStatic("./static");
var httpPort = 8000;

var server = http.createServer(function(request, response) {
    var done = finalhandler(request, response);
    serve(request, response, done);
});

server.listen(httpPort, function() {
    console.log(util.format("Server is listening on port %d.", httpPort));
});

var app = new web.App(server);

function connect (webapp, device) {
    for (var prop in device)
        console.log(prop);
    device.on("frameRequest", function () {
        console.log("frame request");
        device.write(webapp.currentFrame);
    });
}

hardware.find(function (screen) {
    if (screen) {
        console.log("Found hardware.");
        console.log(screen.port.comName);

        connect(app, screen);
    } else {
        console.log("Hardware not found.");
        console.log("Substituting local stdio.");

        connect(app, new VirtualScreen());
    }
});

function VirtualScreen () {
    setInterval((function () {
        this.emit("frameRequest");
    }).bind(this), 100);
}

util.inherits(VirtualScreen, EventEmitter);

VirtualScreen.prototype.write = function (frame) {
    var x = 0;
    var y = 0;

    var drawBuffer = "";

    for (var i = 0; i < frame.length; i++) {
        for (var bit = 0; bit < 8; bit++) {
            if (frame[i] & (0x80 >> bit)) {
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
}

// TODO flags: --no-calc --port

/*serialPort.on("data", function(data) {
    console.log("serial: ", data);

    if (lastFrame !== null)
    serialPort.write(lastFrame);

    lastTime = (new Date()).getTime();
})*/
