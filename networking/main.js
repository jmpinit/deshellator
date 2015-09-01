var util = require("util");
var http = require('http');
var finalhandler = require('finalhandler');
var serveStatic = require('serve-static');

var port = 8000;

var serve = serveStatic("./static");

var server = http.createServer(function(request, response) {
    var done = finalhandler(request, response);
    serve(request, response, done);
});

server.listen(port, function() {
    console.log(util.format("Server is listening on port %d.", port));
});
