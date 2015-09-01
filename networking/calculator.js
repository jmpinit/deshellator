var serialport = require("serialport");

var port = null;

try {
    serialport.list(function (err, ports) {
        ports.forEach(function(port) {
            console.log(port.comName);
            console.log(port.pnpId);
            console.log(port.manufacturer);
        });
    });

    /*var serialPort = new SerialPort("COM37", {
        baudrate: 500000
    });

    serialPort.on("data", function(data) {
        console.log("serial: ", data);

        if (lastFrame !== null)
        serialPort.write(lastFrame);

        lastTime = (new Date()).getTime();
    })*/
} catch (e) {
    console.log("serial port not set up");
    console.log(e);
}
