    #include "echo.h"

#include <muduo/base/Logging.h>
#include <muduo/net/EventLoop.h>
#include <stdio.h>
#include <muduo/base/Mutex.h>
#include <muduo/base/ThreadLocalSingleton.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpServer.h>

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

#include <set>
using namespace muduo;
using namespace muduo::net;

int main(int argc, char* argv[])
{
     muduo::Logger::setLogLevel(muduo::Logger::NUM_LOG_LEVELS);
    LOG_INFO << "pid = " << getpid();

    if (argc > 1)
    {
        EventLoop loop;
        uint16_t port = static_cast<uint16_t>(atoi(argv[1]));
        InetAddress serverAddr(port);
        EchoServer server(&loop,serverAddr);
        if (argc > 2)
        {
             server.setThreadNum(atoi(argv[2]));
        }
        server.start();
        loop.loop();
    }
    else
    {
        printf("Usage: %s port [thread_num]\n", argv[0]);
    }

}

