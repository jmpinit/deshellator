var util = require('util'),
    EventEmitter = require('events').EventEmitter,
    serialport = require("serialport");
var SerialPort = serialport.SerialPort;

var exports = {};

// after this many millis we send another frame
// regardless of whether it was requested
var TIMEOUT = 1000;
var FRAME_REQUEST = "a"; // what the hardware sends to request a frame

var settings = {
    baudrate: 500000
};

function Screen (port) {
    this.port = port;

    this.port.on("data", (function (data) {
        if (data === FRAME_REQUEST) {
            this._restartTimeout();
            this.emit("frameRequest");
        }
    }).bind(this));

    this._restartTimeout();
}

Screen.prototype = {
    write: function (data) {
        if (this.port.isOpen()) {
            this.port.write(data);
        }
    },
    _restartTimeout: function () {
        clearTimeout(this.timeout);
        this.timeout = setTimeout(function () {
            this.emit(frame);
        }, TIMEOUT);
    }
}

util.inherits(Screen, EventEmitter);

exports.find = function (callback) {
    var TIMEOUT = 3000;
    var states = [];

    function endsWith(str, suffix) {
        return str.indexOf(suffix, str.length - suffix.length) !== -1;
    }

    function allChecked () {
        for (var i in states) {
            if (!states[i].checked) {
                return false;
            }
        }

        return true;
    }

    function cleanup (chosen, callback) {
        for (var i in states) {
            clearTimeout(states[i].timeout);
        }

        if (chosen) {
            chosen.close(function () {
                callback();
            })
        } else {
            callback();
        }
    }

    function choose (port) {
        cleanup(port, function () {
            if (port) {
                callback(new Screen(state.port.comName));
            } else {
                callback(null);
            }
        });
    }

    serialport.on("error", function (err) {
        console.log(err);
    });

    serialport.list(function (err, ports) {
        if (err) {
            throw err;
        }

        if (ports.length == 0) {
            callback(null);
        } else {
            ports.forEach(function(portInfo) {
                var state = {
                    port: new SerialPort(portInfo.comName, settings),
                    checked: false,
                    response: "",
                    timeout: null
                };

                states.push(state);

                setTimeout(function () {
                    state.checked = true;

                    for (var i in states) {
                        if (!states[i].checked)
                        return;
                    }

                    console.log("timeout", state.port.comName);

                    // no ports are connected
                    choose(null);
                }, TIMEOUT);

                state.port.on("data", function (data) {
                    state.response += data;

                    if (state.response.endsWith("polo\r\n")) {
                        choose(state.port);
                    }
                });

                state.port.on("open", function (err) {
                    state.port.write("marco");
                });
            });
        }
    });
}

module.exports = exports;
