#include <muduo/base/Logging.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpClient.h>
#include <muduo/net/InetAddress.h>

#include <unistd.h>
#include <iostream>

using namespace muduo;
using namespace muduo::net;

class JsonForwardClient {
public:
    JsonForwardClient(EventLoop* loop, const InetAddress& serverAddr, int php_socket_fd)
        : client_(loop, serverAddr, "JsonForwardClient"),
          php_socket_fd_(php_socket_fd) {
        client_.setConnectionCallback(std::bind(&JsonForwardClient::onConnection, this, _1));
        client_.setMessageCallback(std::bind(&JsonForwardClient::onMessage, this, _1, _2, _3));
    }

    void connect() {
        client_.connect();
    }

private:
    void onConnection(const TcpConnectionPtr& conn) {
        LOG_INFO << "JsonForwardClient - " << conn->localAddress().toIpPort() << " -> "
                 << conn->peerAddress().toIpPort() << " is "
                 << (conn->connected() ? "UP" : "DOWN");

        if (conn->connected()) {
            conn->setTcpNoDelay(true);

            char buf[1024];
            ssize_t n = ::read(php_socket_fd_, buf, sizeof buf);
            if (n > 0) {
                conn->send(buf, n);
            } else {
                LOG_ERROR << "Failed to read data from PHP socket";
                client_.disconnect();
            }
        }
    }

    void onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp receiveTime) {
        muduo::string msg(buf->retrieveAllAsString());
        LOG_INFO << "JsonForwardClient received: " << msg << " from " << conn->name();

        ssize_t n = ::write(php_socket_fd_, msg.data(), msg.size());
        if (n != static_cast<ssize_t>(msg.size())) {
            LOG_ERROR << "Failed to send data back to PHP";
        }

        client_.disconnect();
    }

    TcpClient client_;
    int php_socket_fd_;
};

int main(int argc, char* argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <server_ip> <server_port> <php_socket_fd>\n", argv[0]);
        exit(1);
    }

    InetAddress serverAddr(argv[1], static_cast<uint16_t>(atoi(argv[2])));
    int php_socket_fd = atoi(argv[3]);

    EventLoop loop;
    JsonForwardClient client(&loop, serverAddr, php_socket_fd);
    client.connect();
    loop.loop();

    return 0;
}
