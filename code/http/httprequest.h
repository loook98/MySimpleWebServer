#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <unordered_map>
#include <unordered_set>
#include <string>
#include <regex>
#include <errno.h>
#include <mysql/mysql.h>

#include "../buffer/buffer.h"
#include "../log/log.h"
#include "../sqlconnpool/sql_conn_pool.h"
#include "../threadpool/threadpool.h"

using namespace std;

enum HTTP_CODE
{
    NO_REQUEST = 0,  //报文不完整
    GET_REQUEST,     //已经解析到完整的报文
    BAD_REQUEST,
    NO_RESOURSE,
    FORBIDDENT_REQUEST,
    FILE_REQUEST,
    INTERNAL_ERROR,
    CLOSED_CONNECTION
};

enum PARSE_STATE
{
    REQUEST_LINE = 0,
    HEADERS,
    BODY,
    FINISH
};

class HttpRequest
{
public:
    HttpRequest() {init();}
    ~HttpRequest() = default;

    void init();
    HTTP_CODE parse(Buffer& buffer);

    string getPathConst() const {return path;}
    string& getPath() {return path;}
    string getMethod() const {return method;}
    string getVersion() const {return version;}
    bool isKeepAlive() const;

private:
    HTTP_CODE parseRequestLine(const string& line);
    HTTP_CODE parseHeader(const string& line);
    HTTP_CODE parseBody();

    void parsePath();
    void parseFromUrlEncoded();
    void parseFormData();

    static bool userVerify(const string& name, const string& pwd, bool isLogin);

    PARSE_STATE state; //主状态机的状态，也就是在解析请求行、请求头 还是 消息体。
    string method, path, version, body;
    bool linger; //是否是长连接
    size_t contentLen; //消息体的长度
    unordered_map<string, string> header; //存储从报文中解析出的请求头的“键-值”对。
    unordered_map<string, string> post;   //应该是post消息体中存放的登录信息吧
    unordered_map<string, string> fileInfo; //记录post消息体内上传的文件的信息

    static const unordered_set<string> DEFAULT_HTML;
    static const unordered_map<string, int> DEFAULT_HTML_TAG;
    static int convertHex(char ch);
};

#endif