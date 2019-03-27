#ifndef __M_UNTILS_H__
#define __M_UNTILS_H__
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <dirent.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>

#define MAX_BUFF     4096
#define MAX_HTTPHEAD 4096 
#define WWWROOT     "www"
#define CHECK_OUT(q) do{ if ((q) == false) goto out; }while(0)
#define CHECK_RET(q) do{ if ((q) == false) return false; }while(0)
#define LOG(...) do{ fprintf(stderr, __VA_ARGS__); fflush(stderr); }while(0)
#define HEX(x)   (((x) <= '9') ? ((x) - '0') : (((x) <= 'F') ? ((x) - 'A' + 10) : ((x) - 'a' + 10)))
std::unordered_map<std::string, std::string> g_mime_type = {
    {"txt",     "text/plain"},
    {"html",    "text/html"},
    {"htm",     "text/html"},
    {"jpg",     "image/jpeg"},
    {"zip",     "application/zip"},
    {"mp3",     "audio/mpeg"},
    {"mpeg",    "video/mpeg"},
    {"unknow",  "application/octet-stream"},
};
std::unordered_map<std::string, std::string> g_code_desc = {
    {"200",     "OK"},                              //正常处理
    {"206",     "Partial Content"},                 //分块传输
    {"302",     "Found"},                           //临时重定向
    {"304",     "Not Modified"},                    //资源未修改无需重传，搭配Last_Modified/Etag
    {"400",     "Bad Request"},                     //请求语法错误
    {"403",     "Forbidden"},
    {"404",     "Page Not Found"},                  //无资源
    {"405",     "Method Not Allowed"},              //请求方法被禁止
    {"412",     "Precondition Failed"},             //请求信息先决条件错误
    {"414",     "Request-URI Too Large"},           //uri（网址）过大
    {"416",     "Requested range not satisfiable"}, //分块传输，请求范围不合法
    {"500",     "Internal Server Error"},           //服务器内部错误
};

class Utils
{
    public:
        static int Split(const std::string &src, const std::string &seg, std::vector<std::string> &list) {
            int num = 0;
            size_t spos= 0, epos;
            while(1) {
                epos = src.find(seg, spos);
                if (epos == std::string::npos) {
                    break;
                }
                if (spos != epos) {
                    num++;
                    list.push_back(src.substr(spos, epos - spos));
                }
                spos = epos + seg.length();
            }
            if (spos < (src.length() - 1)) {
                num++;
                list.push_back(src.substr(spos));
            }
            return num;
        }
        static void DigitToStr(int64_t len, std::string &str) {
            std::stringstream ss;
            ss << len;
            ss >> str;
        }
        static int64_t StrToDigit(const std::string &str) {
            int64_t num;
            std::stringstream ss;
            ss << str;
            ss >> num;
            return num;
        }
        static void TimeToGMT(int64_t t, std::string &str) {
            char tmp[128] = {0};
            struct tm *gmt = gmtime(&t);
            int ret = strftime(tmp, 128, "%a, %d %b %Y %H:%M:%S GMT", gmt);
            str.assign(tmp, ret);
        }
        static int64_t GMTToTime(const std::string &str) {
            struct tm gmt;
            if (strptime(str.c_str(), "%a, %d %b %Y %H:%M:%S %Z", &gmt) == NULL) {
                std::cout<<"Parse GMT To Time Error\n";
                return 0;
            }
            return timegm(&gmt);
        }
        static void FileMime(const std::string &file, std::string &mime) {
            size_t pos = file.find_last_of('.');
            if (pos == std::string::npos) {
                mime = g_mime_type["unknow"];
                return;
            }
            auto it = g_mime_type.find(file.substr(pos + 1));
            if (it == g_mime_type.end()) {
                mime = g_mime_type["unknow"];
                return;
            }
            mime = it->second;
        }
        static void MakeEtage(struct stat &st, std::string &etag) {
            std::stringstream ss;
            ss << "\"";
            ss << std::hex << st.st_ino;
            ss << "-";
            ss << std::hex << st.st_size;
            ss << "-";
            ss << std::hex << st.st_mtime;
            ss << "\"";
            etag = ss.str();
        }
        static void UrlDecode(const std::string &src, std::string &dest) {
            char tmp[256] = {0};
            int i = 0, idx = 0;
            while(i < src.length()) {
                if (src[i] == '%') {
                    dest.push_back((char)(HEX(src[i + 1]) * 16 + HEX(src[i + 2])));
                    tmp[idx++] = (char)(HEX(src[i + 1]) * 16 + HEX(src[i + 2]));
                    i += 2;
                }else {
                    dest.push_back(src[i]);
                    tmp[idx++] = src[i];
                }   
                i++;
            }
        }
};
class HttpHeader
{
    public:
        std::string _method;
        std::string _version;
        std::string _path_info;
        std::string _query_str;
        std::unordered_map<std::string, std::string> _hdrs_list;
    public:
        std::string _path_phys;
        struct stat _st;
    public:
        //_err_code SetErrCode(std::string &code)
        std::string _err_code;
        void SetErrCode(const std::string &code) {
            //设置错误状态码
            _err_code = code;
        }
    private:
        bool IsMethodLegal() {
            //判断请求方法是否合法，当前仅支持GET/POST/HEAD
            if (_method != "GET" && _method != "POST" && _method != "HEAD") {
                std::cout<<"Method Not Allowed\n";
                _err_code = "405";
                return false;
            }
            return true;
        }
        bool IsVersionLegal() {
            //判断协议版本是否合法
            if (_version != "HTTP/0.9" && _version != "HTTP/1.0" && _version != "HTTP/1.1") {
                _err_code = "400";
                return false;
            }
            return true;
        }
        bool IsPathLegal() {
            //判断请求资源路径是否合法
            if (_path_phys.find(WWWROOT) == std::string::npos) {
                std::cout<<"Resource Forbidden\n";
                _err_code = "403";
                return false;
            }
            return true;
        }
        bool HeaderInit() {
            //初始化请求资源的实际路径，获取请求资源的文件状态信息，
            std::string full_path = WWWROOT + _path_info;
            char resolved_path[PATH_MAX] = {0};
            if (!realpath(full_path.c_str(), resolved_path)) {
                LOG("Path:[%s] Invalid\n", full_path.c_str());
                _err_code = "404";
                return false;
            }
            _path_phys = resolved_path;
            if (stat(_path_phys.c_str(), &_st) < 0) {
                LOG("Page Not Found\n");
                _err_code = "404";
                return false;
            }
            if (_st.st_mode & S_IFDIR) {
                if (_path_phys[_path_phys.length() - 1] != '/') {
                    _path_phys.push_back('/');
                }
                if (_path_info[_path_info.length() - 1] != '/') {
                    _path_info.push_back('/');
                }
            }
            return true;
        }
    public:
        void AddHeader(const std::string &key, const std::string &val) {
            //添加http头信息的键值对
            _hdrs_list[key] = val;
        }
        bool QueryIsEmpty() {
            //判断query_string是否为空
            return _query_str.empty();
        }
        bool FileIsDirectory() {
            //判断请求资源文件是否是目录
            if (!(_st.st_mode & S_IFDIR)) {
                return false;
            }
            return true;
        }
        bool FileIsExecutable() {
            //判断请求资源文件是否可执行
            if (_st.st_mode & S_IXOTH) {
                return true;
            }
            return false;
        }
        bool RequestIsCGI() {
            //判断请求类型是否是CGI请求
            if ((_method == "GET" && !_query_str.empty()) || _method == "POST") {
                return true;
            }
            return false;
        }
        bool HeaderIsLegal() {
            //判断http头部是否合法
            CHECK_RET(HeaderInit());
            CHECK_RET(IsPathLegal());
            CHECK_RET(IsVersionLegal());
            CHECK_RET(IsMethodLegal());
            return true;
        }
};
class HttpRequest
{
    private:
        int _cli_sock;
        std::string _header;
    private:
        void UrlParse(std::string &url, std::string &path, std::string &query) {
            //解析url，对path进行urldecode ；获取path/query
            size_t pos = url.find("?");
            if (pos == std::string::npos) {
                Utils::UrlDecode(url, path);
            }else {
                Utils::UrlDecode(url.substr(0, pos), path);
                query = url.substr(pos + 1);
            }
        }
        void GetHeader(char *buf, int len) {
            //获取去掉\r之后的http头字符串
            for (int i = 0; i < len; i++) {
                if (buf[i] == '\r') {
                    continue;
                }
                _header.push_back(buf[i]);
            }
        }
    public:
        HttpRequest(int sock): _cli_sock(sock) {}
        bool RecvHttpHeader(HttpHeader *hdrs) {
            //接收HTTP数据
            char tmp[MAX_HTTPHEAD] = {0};
            int rlen = 0;
            while(1) {
                rlen = recv(_cli_sock, tmp, MAX_HTTPHEAD, MSG_PEEK);
                if (rlen <= 0) {
                    if (errno == EINTR || errno == EAGAIN) {
                        continue;
                    }
                    LOG("recv http head error\n");
                    hdrs->SetErrCode("400");
                    return false;
                }
                char *pos;
                if ((pos = strstr(tmp, "\r\n\r\n")) || (pos = strstr(tmp, "\n\n"))) {
                    GetHeader(tmp, pos - tmp);
                    recv(_cli_sock, tmp, pos - tmp + 4, 0);
                    break;
                }
                if (rlen == MAX_HTTPHEAD) {
                    LOG("Request Entity Too Large\n");
                    hdrs->SetErrCode("413");
                    return false;
                }
            }
            return true;
        }
        //解析头部信息
        bool HttpHeaderParse(HttpHeader *hdrs) {
            //http头解析, 首行/头部/仅解析,不验证
            std::vector<std::string> header_list;
            Utils::Split(_header, "\n", header_list);
            //首行解析
            std::vector<std::string> f_line_list;
            if (Utils::Split(header_list[0], " ", f_line_list) != 3) {
                LOG("First line format error: %s\n", header_list[0].c_str());
                hdrs->SetErrCode("400");
                return false;
            }
            //url解析
            UrlParse(f_line_list[1], hdrs->_path_info, hdrs->_query_str);
            hdrs->_method = f_line_list[0];
            hdrs->_version = f_line_list[2];
            //头部分割建立键值对
            for(int i = 1; i < header_list.size(); i++) {
                size_t pos = header_list[i].find(": ");
                if (pos == std::string::npos) {
                    LOG("Http Header Parse Error\n");
                    hdrs->SetErrCode("400");
                    return false;
                }
                hdrs->AddHeader(header_list[i].substr(0, pos), header_list[i].substr(pos + 2));
            }
            return true;
        }
};
class HttpResponse
{
    private:
        int _cli_sock;
        bool _is_range;
        int64_t _cont_len;
        std::string _version;
        std::string _etag;  //资源版本标识符
        std::string _mtime; //资源最后修改时间
        std::string _rsp_time;  //响应时间
        std::string _mime;  //资源扩展类型
        std::string _flen;  //资源大小
        std::unordered_map<int64_t, int64_t> _range_list;//分块传输，各个块区域
    private:
        bool SendData(const char *buf, int len) {
            //常规发送数据
            int ret = send(_cli_sock, buf, len, 0);
            if (ret < 0) {
                return false;
            }
            return true;
        }
        bool SendCData(const std::string &buf) {
            //根据协议版本判断是否进行chunked发送数据
            if (_version == "HTTP/1.1") {
                if (buf.empty()) {
                    CHECK_RET(SendData("0\r\n\r\n", 5));
                    return true;
                }
                std::stringstream ss;
                ss << std::hex << buf.length() << "\r\n";
                CHECK_RET(SendData(ss.str().c_str(), ss.str().length()));
                CHECK_RET(SendData(buf.c_str(), buf.length()));
                CHECK_RET(SendData("\r\n", 2));
                return true;
            }
            CHECK_RET(SendData(buf.c_str(), buf.length()));
            return true;
        }
    private:
        bool FileIfModefiedSince(HttpHeader *hdrs) {
            //资源若在给定的时间之前未修改则响应304；否则响应200
            auto it = hdrs->_hdrs_list.find("If-Modified-Since");
            if ((it != hdrs->_hdrs_list.end()) && 
                    (Utils::GMTToTime(it->second) >= Utils::GMTToTime(_mtime))) {
                hdrs->SetErrCode("304");
                return false;
            }
            return true;
        }
        bool FileIfUnModefiedSince(HttpHeader *hdrs) {
            //资源若在给定日期之后修改过，则响应412；否则响应200
            auto it = hdrs->_hdrs_list.find("If-Unmodified-Since");
            if ((it != hdrs->_hdrs_list.end()) && 
                    (Utils::GMTToTime(it->second) <= Utils::GMTToTime(_mtime))) {
                hdrs->SetErrCode("412");
                return false;
            }
            return true;
        }
        bool FileIfMatch(HttpHeader *hdrs) {
            //资源的etag若与给定的etag不匹配，则响应412；否则响应200
            auto it = hdrs->_hdrs_list.find("If-Match");
            if ((it != hdrs->_hdrs_list.end()) && (it->second != _mtime)) {
                hdrs->SetErrCode("412");
                return false;
            }
            return true;
        }
        bool FileIfNoneMatch(HttpHeader *hdrs) {
            //资源etag若与给定etag匹配,且请求方法为GET/HEAD则响应304,否则响应412;反之响应200
            auto it = hdrs->_hdrs_list.find("If-None-Match");
            if (it != hdrs->_hdrs_list.end()) {
                if (it->second == _etag) {
                    if (hdrs->_method == "GET" || hdrs->_method == "HEAD") {
                        hdrs->SetErrCode("304");
                    }else {
                        hdrs->SetErrCode("412");
                    }
                    return false;
                }
            }
            return true;
        }
        bool FileIfRange(HttpHeader *hdrs) {
            //资源的etag/last-modified与给定的if-range相匹配，则range生效；响应206；否则响应200
            auto it = hdrs->_hdrs_list.find("If-Range");
            if ((it != hdrs->_hdrs_list.end()) && it->second != _mtime) {
                hdrs->SetErrCode("412");
                return false;
            }
            return true;
        }
        bool FileRange(HttpHeader *hdrs) {
            //资源的大小符合请求的块范围，则响应206与对应块数据；否则响应416/200
            auto it = hdrs->_hdrs_list.find("Range");
            if (it == hdrs->_hdrs_list.end()) {
                //若未找到Range字段，则认为发送范围从文件起始-结束
                _is_range = false;
                _range_list[0] = Utils::StrToDigit(_flen) - 1;
                return true;
            }
            _is_range = true;
            std::string full_range = it->second;
            size_t pos = full_range.find("=");
            if (pos == std::string::npos) {
                hdrs->SetErrCode("416");
                return false;
            }
            std::string unit = full_range.substr(0, pos);
            std::string range = full_range.substr(pos + 1);

            //这里需要注意的是Range中有可能会有多个范围以,间隔，则需要取出多个范围分别传输
            std::vector<std::string> range_list;
            int range_num = Utils::Split(range, ",", range_list);
            for (int i = 0; i < range_num; i++) {
                if (range_list[i].length() == 1) {
                    hdrs->SetErrCode("416");
                    return false;
                }
                pos = range_list[i].find("-");
                if (pos == std::string::npos) {
                    hdrs->SetErrCode("416");
                    return false;
                }
                int64_t s = 0, e = 0, t = 0;
                int64_t fend= Utils::StrToDigit(_flen) - 1;
                if (pos == 0) {
                    //Range: bytes=-500 最后500个字节
                    t = Utils::StrToDigit(range_list[i].substr(pos + 1));
                    e = fend;
                    s = e - t + 1;
                }else if (pos == (range_list[i].length() - 1)) {
                    //std::cout<<"range: bytes start-\n"; 指定位置到结束
                    s = Utils::StrToDigit(range_list[i].substr(0, pos));
                    e = fend;
                }else {
                    //std::cout<<"range: bytes=start-end\n"; 指定起始-结束位置
                    s = Utils::StrToDigit(range_list[i].substr(0, pos));
                    e = Utils::StrToDigit(range_list[i].substr(pos + 1));
                }
                if (s < 0 || s > fend || e > fend || s > e) {
                    hdrs->SetErrCode("416");
                    return false;
                }
                _range_list[s] = e;
            }

            return true;
        }
    private:
        bool InitResponse(HttpHeader *hdrs) {
            //初始化请求资源响应信息：ETag Last-Modified Date FileSize Content-Len Version Mime
            Utils::MakeEtage(hdrs->_st, _etag);
            Utils::TimeToGMT(hdrs->_st.st_mtime, _mtime);
            Utils::TimeToGMT(time(NULL), _rsp_time);
            Utils::DigitToStr(hdrs->_st.st_size, _flen);
            Utils::FileMime(hdrs->_path_phys, _mime);
            auto it = hdrs->_hdrs_list.find("Content-Length");
            if (it != hdrs->_hdrs_list.end()) {
                _cont_len = Utils::StrToDigit(it->second);
            }else {
                _cont_len = 0;
            }
            _version = hdrs->_version;
            return true;
        }
        bool ProcessFile(HttpHeader *hdrs) {
            //文件下载请求处理
            LOG("File Donwload!\n");
            std::string rsp_header;
            for (auto it = _range_list.begin(); it != _range_list.end(); it++) {
                std::string s_pos;
                std::string e_pos;
                std::string content_len;
                int64_t spos = it->first;
                int64_t epos = it->second;
                int64_t clen = it->second - it->first + 1;
                Utils::DigitToStr(spos, s_pos);
                Utils::DigitToStr(epos, e_pos);
                Utils::DigitToStr(clen, content_len);
                if (_is_range) {
                    rsp_header = hdrs->_version + " 206 " + g_code_desc["206"] +"\r\n"; 
                    rsp_header += "Content-Range: bytes " + s_pos + "-" + e_pos + "/" + _flen + "\r\n";
                }else {
                    rsp_header = hdrs->_version + " 200 " + g_code_desc["200"] +"\r\n"; 
                }
                rsp_header += "Content-length: " + content_len + "\r\n";
                rsp_header += "Accept-Ranges: bytes\r\n";
                rsp_header += "Content-Type: " + _mime + "\r\n";
                rsp_header += "Connection: close\r\n";
                rsp_header += "ETag: " + _etag + "\r\n";
                rsp_header += "Last-Modified: " + _mtime + "\r\n";
                rsp_header += "Date: " + _rsp_time + "\r\n\r\n";
                CHECK_RET(SendData(rsp_header.c_str(), rsp_header.length()));

                int file_fd = open(hdrs->_path_phys.c_str(), O_RDONLY);
                if (file_fd < 0) {
                    LOG("Access to this resource is forbidden\n");
                    hdrs->SetErrCode("403");
                    return false;
                }
                lseek(file_fd, spos, SEEK_SET);
                int64_t slen = 0, ret, rlen;
                char buf[MAX_HTTPHEAD];
                while(slen < clen) {
                    rlen = (clen - slen) > MAX_HTTPHEAD ? MAX_HTTPHEAD : (clen - slen);
                    if ((ret = read(file_fd, buf, rlen)) < 0) {
                        if (errno == EINTR || errno == EAGAIN) {
                            continue;
                        }
                        return false;
                    }
                    CHECK_RET(SendData(buf, ret));
                    slen += ret;
                }
            }
        }
        bool ProcessList(HttpHeader *hdrs) {
            //目录列表请求处理
            LOG("File List Display!\n");
            std::string rsp_header;
            rsp_header = hdrs->_version + " 200 " + g_code_desc["200"] +"\r\n"; 
            rsp_header += "Connection: close\r\n";
            rsp_header += "Content-Type: text/html\r\n";
            rsp_header += "Content-Disposition: inline\r\n";
            if (hdrs->_version == "HTTP/1.1") {
                rsp_header += "Transfer-Encoding: chunked\r\n";
            }
            rsp_header += "ETag: " + _etag + "\r\n";
            rsp_header += "Last-Modified: " + _mtime + "\r\n";
            rsp_header += "Date: " + _rsp_time + "\r\n\r\n";
            CHECK_RET(SendData(rsp_header.c_str(), rsp_header.length()));

            std::string rsp_body;
            rsp_body = "<html><head><title>Index of " + hdrs->_path_info + "</title>";
            rsp_body += "<meta charset='UTF-8'></head>";
            rsp_body += "<body><h1>Index of " + hdrs->_path_info;
            rsp_body += "<form method='post' action='/upload' enctype='multipart/form-data'>";
            rsp_body += "<input type='text' name='username' value='zhang' />";
            rsp_body += "<input type='file' name='file1' />";
            rsp_body += "<input type='file' name='file2' />";
            rsp_body += "<input type='submit' value='上传文件' />";
            rsp_body += "</form></h1><hr /><ol>";
            CHECK_RET(SendCData(rsp_body));
            struct dirent **p_dirent = NULL;
            int sub_file_num = scandir(hdrs->_path_phys.c_str(), &p_dirent, 0, alphasort);
            for (int i = 0; i < sub_file_num; i++) {
                std::string name = p_dirent[i]->d_name;
                std::string file = hdrs->_path_phys + name;
                struct stat st;
                if (stat(file.c_str(), &st) < 0) {
                    continue;
                }
                if ((!(st.st_mode & S_IXOTH) && (st.st_mode & S_IFDIR)) ||
                        (!(st.st_mode & S_IROTH) && !(st.st_mode & S_IFDIR))) {
                    //防止有的文件没有权限，干脆就不显示了：目录不能进入，文件不能读取
                    continue;
                }
                std::string mtime;
                std::string fsize;
                std::string mime;
                Utils::TimeToGMT(st.st_mtime, mtime);
                Utils::DigitToStr(st.st_size / 1024, fsize);
                if (st.st_mode & S_IFDIR) {
                    name.push_back('/');
                    mime = "directory";
                }else {
                    Utils::FileMime(name, mime);
                }
                rsp_body = "<li><strong><a href='" + hdrs->_path_info + name + "'>" + name + "</a></strong><br />";
                rsp_body += "<small>modified: " + mtime + "<br />" + mime + " - " + fsize + " kbytes<br /><br /></small></li>";
                CHECK_RET(SendCData(rsp_body));
            }
            rsp_body = "</ol><hr /></body></html>";
            CHECK_RET(SendCData(rsp_body));
            CHECK_RET(SendCData(""));
            if (p_dirent) {
                for (int i = 0; i < sub_file_num; i++) {
                    free(p_dirent[i]);
                }
                free(p_dirent);
            }
        }
        bool RecvContentToCGI(int pipefd, HttpHeader *hdrs) {
            //像cgi程序写入正文
            int rlen = 0, ret, tlen = 0;
            char buf[MAX_HTTPHEAD];
            while(tlen < _cont_len) {
                rlen = (_cont_len - tlen) > MAX_HTTPHEAD ? MAX_HTTPHEAD : (_cont_len - tlen);
                ret = recv(_cli_sock, buf, rlen, 0);
                if (ret <= 0) {
                    if (errno == EINTR || errno == EAGAIN) {
                        continue;
                    }
                    return false;
                }
                if (write(pipefd, buf, ret) < 0) {
                    return false;
                }
                tlen += ret;
            }
            return true;
        }
        bool RespContentFromCGI(int pipefd, HttpHeader *hdrs) {
            //从cgi程序读取结果
            int ret;
            std::string rsp_header;
            std::string date;
            Utils::TimeToGMT(time(NULL), date);
            rsp_header = hdrs->_version + " 200 OK\r\n";
            rsp_header += "Connection: close\r\n";
            rsp_header += "Content-Type: text/html\r\n";
            rsp_header += "Date: " + date + "\r\n\r\n";
            CHECK_RET(SendData(rsp_header.c_str(), rsp_header.length()));
            char buf[MAX_HTTPHEAD];
            while((ret = read(pipefd, buf, MAX_HTTPHEAD)) > 0) {
                CHECK_RET(SendData(buf, ret));
            }
            return true;
        }
    public:
        //对外接口 ErrHandler CGIHandler FileHandler
        HttpResponse(int sock):_cli_sock(sock),_is_range(false), _cont_len(0) {}
        bool ErrHandler(HttpHeader *hdrs) {
            //处理错误响应
            std::string rsp_header;
            std::string rsp_body;
            std::string date;
            Utils::TimeToGMT(time(NULL), date);
            rsp_header = hdrs->_version + " ";
            rsp_header += hdrs->_err_code + " ";
            rsp_header += g_code_desc[hdrs->_err_code] + "\r\n";
            rsp_header += "Connection: close\r\n";
            rsp_header += "Content-Type: text/html\r\n";
            rsp_header += "Date: " + date + "\r\n";

            rsp_body = "<html><body><h1>";
            switch(Utils::StrToDigit(hdrs->_err_code)) {
                case 304:
                    rsp_header += "Etag: " + _etag + "\r\n";
                    rsp_header += "Last-Modified: " + _mtime + "\r\n";
                case 400:
                    rsp_body += "Bad Request";
                    break;
                case 403:
                    rsp_body += "Forbidden";
                    break;
                case 404:
                    rsp_body += "Page Not Found";
                    break;
                case 405:
                    rsp_body += "Method Not Allowed";
                    break;
                case 412:
                    rsp_body += "Precondition Failed";
                    rsp_header += "ETag: " + _etag + "\r\n";
                    rsp_header += "Last-Modified: " + _mtime + "\r\n";
                    break;
                case 414:
                    rsp_body += "Request-URI Too Large";
                    break;
                case 416:
                    rsp_body += "Requested range not satisfiable";
                    break;
                case 500:
                    rsp_body += "Server Error";
                    break;
                default :
                    return false;
            }
            rsp_header += "\r\n";
            CHECK_RET(SendData(rsp_header.c_str(), rsp_header.length()));

            rsp_body += "</h1></body></html>";
            if (hdrs->_err_code != "304" && hdrs->_err_code != "302") {
                CHECK_RET(SendData(rsp_body.c_str(), rsp_body.length()));
                LOG("rsp_body:[%s]\n\n", rsp_body.c_str());
            }
            return true;
        }
        bool CGIHandler(HttpHeader *hdrs) {
            //处理CGI响应
            if (!hdrs->FileIsExecutable()) {
                //判断请求资源是否可执行
                LOG("Have Not Execute Permision\n");
                hdrs->SetErrCode("403");
                return false;
            }
            CHECK_RET(InitResponse(hdrs));
            int in[2];//父进程写，子进程读
            int out[2];//父进程读，子进程写
            if (pipe(in) < 0 || pipe(out) < 0) {
                //创建管道
                LOG("Create Pipe Error\n");
                hdrs->SetErrCode("500");
                return false;
            }
            int pid = fork();
            if (pid < 0) {
                //创建管道失败
                LOG("Fork Child Error\n");
                hdrs->SetErrCode("500");
                return false;
            }else if (pid == 0) {
                //设置环境变量，进行cgi程序替换
                setenv("METHOD", hdrs->_method.c_str(), 1);
                setenv("PATH_INFO", hdrs->_path_info.c_str(), 1);
                setenv("VERSION", hdrs->_version.c_str(), 1);
                setenv("QUERY_STRING", hdrs->_query_str.c_str(), 1);
                for (auto it = hdrs->_hdrs_list.begin(); it != hdrs->_hdrs_list.end(); it++) {
                    setenv(it->first.c_str(), it->second.c_str(), 1);
                }
                close(in[1]);
                close(out[0]);
                dup2(in[0], 0);
                dup2(out[1], 1);
                execlp(hdrs->_path_phys.c_str(), hdrs->_path_phys.c_str(), NULL);
                LOG("Child Exec Error\n");
                exit(0);
            }
            close(in[0]);
            close(out[1]);
            //接收客户端正文通过管道in传递给cgi程序
            CHECK_RET(RecvContentToCGI(in[1], hdrs));
            //通过out管道从cgi程序读取处理结果响应给客户端
            CHECK_RET(RespContentFromCGI(out[0], hdrs));
            close(in[1]);
            close(out[0]);
            return true;
        }
        bool FileHandler(HttpHeader *hdrs) {
            //处理文件响应
            CHECK_RET(InitResponse(hdrs));
            CHECK_RET(FileIfModefiedSince(hdrs));
            CHECK_RET(FileIfUnModefiedSince(hdrs));
            CHECK_RET(FileIfMatch(hdrs));
            CHECK_RET(FileIfNoneMatch(hdrs));
            CHECK_RET(FileRange(hdrs));
            if (hdrs->FileIsDirectory()) {
                ProcessList(hdrs);
            }else {
                ProcessFile(hdrs);
            }
            return true;
        }
};

#endif
