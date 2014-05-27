var net = require('net');

var HOST = 'localhost';
var PORT = 13513;

// 创建一个TCP服务器实例，调用listen函数开始监听指定端口
// 传入net.createServer()的回调函数将作为”connection“事件的处理函数
// 在每一个“connection”事件中，该回调函数接收到的socket对象是唯一的
net.createServer(function(sock) {

    // 为这个socket实例添加一个"data"事件处理函数
    sock.on('data', function(data) {
        sock.write('+PONG\r\n');
    });

}).listen(PORT, HOST);