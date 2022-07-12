#ifndef HTTPCONNECT_H
#define HTTPCONNECT_H

#include <sys/types.h>
#include <sys/uio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <errno.h>

#include "../log/log.h"
#include "../sqlconnpool/sql_conn_pool.h"
#include "../buffer/buffer.h"
#include "httprequest.h"
#include "httpresponse.h"

using namespace std;

//对外主要调用这个类，httprequest和httpresponse类都是在这个类中使用的。
//处理一个报文的流程 read() ---> process() ---> write()
class HttpConnect
{
public:
    HttpConnect();
    ~HttpConnect() {closeConnect();}

    void init(int sockfd, const sockaddr_in& addr);

    ssize_t read(int* saveErrno); //报文读入buffer,等待处理
    ssize_t write(int* saveErrno); //将buffer中的响应报文发出

    void closeConnect();

    int getfd() const {return fd;}
    sockaddr_in getAddr() const {return addr;}
    int getPort() const {return addr.sin_port;}
    const char* getIP() const {return inet_ntoa(addr.sin_addr);}

    bool process(); //处理buffer中的报文，生成响应报文，写入另一个buffer。

    int toWriteBytes()
    {
        return iov[0].iov_len + iov[1].iov_len;
    }

    bool isKeepAlive() const
    {
        return request.isKeepAlive();
    }

    static bool isET;
    static const char* srcDir;
    static atomic<int> userCnt;

private:
    int fd;
    struct sockaddr_in addr;

    bool isClose;

    int iovCnt;
    struct iovec iov[2];

    Buffer readBuffer;
    Buffer writeBuffer;

    HttpRequest request;
    HttpResponse response;
};

#endif