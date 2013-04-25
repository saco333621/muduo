#include "echo.h"
#include "client.cc"
#include <muduo/base/Logging.h>
#include <muduo/net/EventLoopThread.h>
#include <boost/bind.hpp>

using namespace muduo;
using namespace muduo::net;

EchoServer::EchoServer(muduo::net::EventLoop* loop,
                       const muduo::net::InetAddress& listenAddr)
    : loop_(loop),
      server_(loop, listenAddr, "EchoServer")
{
    server_.setConnectionCallback(
                boost::bind(&EchoServer::onConnection, this, _1));
    server_.setMessageCallback(
                boost::bind(&EchoServer::onMessage, this, _1, _2, _3));
}
void EchoServer::setThreadNum(int numThreads)
{
    server_.setThreadNum(numThreads);
}

void EchoServer::start()
{

    server_.setThreadInitCallback(boost::bind(&EchoServer::threadInit, this, _1)); ///high performance
    server_.start();
    // createClient();
}
void EchoServer::createClient()
{
    EventLoopThread loopThreadC;
    InetAddress serverAddr("127.0.0.1", 80);

    EchoClient client(loopThreadC.startLoop(), serverAddr);
    client.connect();
    std::string line;
    client.write("GET /file2.php HTTP/1.1/r/n/r/n");
    //    client.disconnect();
}

void EchoServer::onConnection(const muduo::net::TcpConnectionPtr& conn)
{
    LOG_INFO << "EchoServer - " << conn->peerAddress().toIpPort() << " -> " << conn->localAddress().toIpPort() << " is "   << (conn->connected() ? "UP" : "DOWN");

    /// high performance
    if (conn->connected())
    {
      LocalConnections::instance().insert(conn);
    }
    else
    {
      LocalConnections::instance().erase(conn);
    }
    return;
    /// threaded
    MutexLockGuard lock(mutex_);
    if (conn->connected())
    {
        connections_.insert(conn);
    }
    else
    {
        connections_.erase(conn);
    }
}

void EchoServer::onMessage(const muduo::net::TcpConnectionPtr& conn,
                           muduo::net::Buffer* buf,
                           muduo::Timestamp time)
{

    //High Performance
    for (ConnectionList::iterator it = LocalConnections::instance().begin();
         it != LocalConnections::instance().end();
         ++it){
        conn->send(buf);
        conn->shutdown();
    }
    return;
    ///Thraded
    MutexLockGuard lock(mutex_);
    for (ConnectionList::iterator it = connections_.begin();
         it != connections_.end();
         ++it)
    {
        //muduo::string msg(buf->retrieveAllAsString());
        // LOG_INFO << conn->name() << " echo " << msg.size() << " bytes, " << "data received at " << time.toString();
        conn->send(buf);
        conn->shutdown();
    }
}
void EchoServer::threadInit(EventLoop* loop)
{
    assert(LocalConnections::pointer() == NULL);
    LocalConnections::instance();
    assert(LocalConnections::pointer() != NULL);
    MutexLockGuard lock(mutex_);
    loops_.insert(loop);
}
