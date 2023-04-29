#include <muduo/base/Logging.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpServer.h>

#include <functional>

using namespace muduo;
using namespace muduo::net;

class JsonForwardServer {
public:
    JsonForwardServer(EventLoop* loop, const InetAddress& listenAddr)
        : server_(loop, listenAddr, "JsonForwardServer") {
        server_.setConnectionCallback(
            std::bind(&JsonForwardServer::onConnection, this, std::placeholders::_1));
        server_.setMessageCallback(
            std::bind(&JsonForwardServer::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    }

    void start() { server_.start(); }

private:
    void onConnection(const TcpConnectionPtr& conn) {
        if (conn->connected()) {
            LOG_INFO << "JsonForwardServer - " << conn->peerAddress().toIpPort() << " -> "
                     << conn->localAddress().toIpPort() << " is UP";
        } else {
            LOG_INFO << "JsonForwardServer - " << conn->peerAddress().toIpPort() << " -> "
                     << conn->localAddress().toIpPort() << " is DOWN";
        }
    }

    void onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp receiveTime) {
        muduo::string msg(buf->retrieveAllAsString());
        LOG_INFO << "JsonForwardServer received: " << msg << " from " << conn->name();
        conn->send(msg);  // 这里简单地将收到的数据原样返回，你可以在这里处理数据并返回结果
    }

    TcpServer server_;
};

int main(int argc, char* argv[]) {
    LOG_INFO << "pid = " << getpid();
    EventLoop loop;
    InetAddress listenAddr(8888);
    JsonForwardServer server(&loop, listenAddr);
    server.start();
    loop.loop();
}

