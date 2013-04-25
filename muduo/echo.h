#ifndef MUDUO_EXAMPLES_SIMPLE_ECHO_ECHO_H
#define MUDUO_EXAMPLES_SIMPLE_ECHO_ECHO_H

#include <muduo/net/TcpServer.h>
#include <muduo/base/Logging.h>
#include <muduo/base/Mutex.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpConnection.h>


#include <boost/bind.hpp>
#include <muduo/base/ThreadLocalSingleton.h>
#include <set>
#include <stdio.h>
//#include "codec.h"
using namespace muduo;
using namespace muduo::net;

class EchoServer : boost::noncopyable
{
public:
    EchoServer(muduo::net::EventLoop* loop,
               const muduo::net::InetAddress& listenAddr);

    void start();  // calls server_.start();
    void setThreadNum(int numThreads);
private:
    void onConnection(const muduo::net::TcpConnectionPtr& conn);

    void onMessage(const muduo::net::TcpConnectionPtr& conn,
                   muduo::net::Buffer* buf,
                   muduo::Timestamp time);
    void createClient();
    void threadInit(EventLoop* loop);

    typedef std::set<muduo::net::TcpConnectionPtr> ConnectionList;
    typedef ThreadLocalSingleton<ConnectionList> LocalConnections;

    EventLoop* loop_;
    TcpServer server_;
    //    LengthHeaderCodec codec_;
    MutexLock mutex_;
    ConnectionList connections_;
    std::set<EventLoop*> loops_;
};

#endif  // MUDUO_EXAMPLES_SIMPLE_ECHO_ECHO_H
