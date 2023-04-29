#include <muduo/base/Logging.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpClient.h>

#include <functional>

using namespace muduo;
using namespace muduo::net;

class JsonForwardClient {
public:
    JsonForwardClient(EventLoop* loop, const InetAddress& serverAddr)
        : client_(loop, serverAddr, "JsonForwardClient") {
        client_.setConnectionCallback(
            std::bind(&JsonForwardClient::onConnection, this, std::placeholders::_1));
        client_.setMessageCallback(
            std::bind(&JsonForwardClient::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    }

    void connect() { client_.connect(); }

private:
    void onConnection(const TcpConnectionPtr& conn) {
        if (conn->connected()) {
            LOG_INFO << "JsonForwardClient - " << conn->localAddress().toIpPort() << " -> "
                     << conn->peerAddress().toIpPort() << " is UP";
            conn->send("JSON数据");  // 在此处发送你从前端PHP接收到的JSON数据
        } else {
            LOG_INFO << "JsonForwardClient - " << conn->localAddress().toIpPort() << " -> "
                     << conn->peerAddress().toIpPort() << " is DOWN";
            client_.disconnect();
        }
    }

    void onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp receiveTime) {
        muduo::string msg(buf->retrieveAllAsString());
        LOG_INFO << "JsonForwardClient received: " << msg << " from " << conn->name();
        // 这里，你可以将收到的数据返回给前端PHP或者前端页面
        client_.disconnect();
    }

    TcpClient client_;
};

int main(int argc, char* argv[]) {
    LOG_INFO << "pid = " << getpid();
    EventLoop loop;
    InetAddress serverAddr("服务器IP地址", 8888); // 请替换为你的服务器IP地址
    JsonForwardClient client(&loop, serverAddr);
    client.connect();
    loop.loop();
}
