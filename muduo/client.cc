#include "codec.h"

#include <muduo/base/Logging.h>
#include <muduo/base/Mutex.h>
#include <muduo/net/EventLoopThread.h>
#include <muduo/net/TcpClient.h>

#include <boost/bind.hpp>
#include <boost/noncopyable.hpp>

#include <iostream>
#include <stdio.h>

using namespace muduo;
using namespace muduo::net;

class EchoClient : boost::noncopyable
{
public:
    EchoClient(EventLoop* loop, const InetAddress& serverAddr)
        : loop_(loop),
          client_(loop, serverAddr, "EchoClient"),
          codec_(boost::bind(&EchoClient::onStringMessage, this, _1, _2, _3))
    {
        client_.setConnectionCallback(boost::bind(&EchoClient::onConnection, this, _1));
        //   client_.setMessageCallback(boost::bind(&LengthHeaderCodec::onMessage, &codec_, _1, _2, _3));
        client_.setMessageCallback(boost::bind(&EchoClient::onMessage, this, _1, _2, _3));
        client_.enableRetry();
    }

    void connect()
    {
        client_.connect();
    }

    void disconnect()
    {
        // client_.disconnect();
    }

    void write(const StringPiece& message)
    {
        MutexLockGuard lock(mutex_);
        if (connection_)
        {
            int64_t message1[2] = { 0, 0 };
            message1[0] = Timestamp::now().microSecondsSinceEpoch();
            message1[1] =55555;
            connection_->send(message1,sizeof(message1));
            return;
            codec_.send(get_pointer(connection_), message);
        }
    }

private:
    void onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp receiveTime)
    {
        if (buf->readableBytes() >= sizeof(int32_t))
        {
            const void* data = buf->peek();
            int32_t be32 = *static_cast<const int32_t*>(data);
            buf->retrieve(sizeof(int32_t));
            time_t time = sockets::networkToHost32(be32);
            Timestamp ts(time * Timestamp::kMicroSecondsPerSecond);
            LOG_INFO << "Server time = " << time << ", " << ts.toFormattedString();
        }
        else
        {
            LOG_INFO << conn->name() << " no enough data " << buf->readableBytes()
                     << " at " << receiveTime.toFormattedString();
        }
    }

    void onConnection(const TcpConnectionPtr& conn)
    {
        LOG_INFO << conn->localAddress().toIpPort() << " -> " << conn->peerAddress().toIpPort() << " is " << (conn->connected() ? "UP" : "DOWN");

        MutexLockGuard lock(mutex_);
        if (conn->connected())
        {
            connection_ = conn;
        }
        else
        {
            connection_.reset();
        }
    }

    void onStringMessage(const TcpConnectionPtr&,
                         const string& message,
                         Timestamp)
    {
        printf("<<< %s\n", message.c_str());
    }

    EventLoop* loop_;
    TcpClient client_;
    LengthHeaderCodec codec_;
    MutexLock mutex_;
    TcpConnectionPtr connection_;
};

//int main(int argc, char* argv[])
//{
//    LOG_INFO << "pid = " << getpid();
//    if (argc > 2)
//    {
//        EventLoopThread loopThread;
//        uint16_t port = static_cast<uint16_t>(atoi(argv[2]));
//        InetAddress serverAddr(argv[1], port);

//        EchoClient client(loopThread.startLoop(), serverAddr);
//        client.connect();
//        std::string line;
//        while (std::getline(std::cin, line))
//        {
//            client.write(line);
//        }
//        client.disconnect();
//    }
//    else
//    {
//        printf("Usage: %s host_ip port\n", argv[0]);
//    }
//}

