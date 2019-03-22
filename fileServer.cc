#include<iostream>

using namespace std;


typedef bool (*Handler)(int sock);

class HttpTask
{
	//http请求处理的任务
	//包含一个成员就是Socket
	//包含一个任务处理接口
	private:
		int _cli_sock;
		Handler TaskHandler;
	public:
        HttpTask():_cli_sock(-1){}
        HttpTask(int sock,Handler handle):_cli_sock(sock),TaskHandler(handle){}

		void SetHttpTask(int sock, Handler handler){
			_cli_sock = sock;
			TaskHandler = handler;
		}

		void Handler(){
			TaskHandler(_cli_sock);
		}
};

class ThreadPool
{
	//线程池
	//创建指定数量的线程
	//创建一个安全的任务队列
	//提供任务的入队，出队，线程池销毁/初始化接口
	private:
		int _max_thr;//当前线程池的最大线程
		int _cur_thr;//当前线程池中的线程数
		std::queue<HttpTask> _task_queue;
		pthread_mutex_t mutex;
		pthread_cond_t _cond;
    private:
		static void *thr_start(void *arg);//完成线程获取任务，
	public:
		ThreadPool(int max) :_max_thr(max){}
		bool ThreadPoolInit();//完成线程创建，互斥锁/条件变量
		bool PushTask(HttpTask &tt);//线程安全的任务入队
		bool PopTask(HttpTask &tt);//线程安全的任务出队
		bool ThreadPoolStop();//销毁线程池
};


///////////////////////////////
#include <iostream>
#include "threadpool.hpp"

#define MAX_LISTEN 5
#define MAX_THREAD 5

class HttpServer
{
	//建立一个TCP服务端程序，接收新连接
	//为新连接组织一个线程池任务，添加到线程池中
	private:
		int _serv_sock;
		ThreadPool *_tp;
	private:
        //http任务的处理函数
		static bool (HttpHandler)(int sock){
            RequestInfo info;
            HttpRequest req(sock);
            HttpRsepose rsp(sock);
            
            
            //就收http头部
            if(req.RecvHttpHeader(info) == fasle){
                goto out;
            }
            //解析http头部
            if(req.ParseHttpHeader(info) == fasle){
                goto out;
            }
            if (info.RequestIsCGI()){
                rsp.CGIHandler(info);
            }else
            {
                rsp.FileHandler(info);
            }
            
            close(sock);        
            return true;
        out:
            rsp.ErrHandler(info);
            close(sock);
            return false;


            return true;
        }
	public:
        HttpServer():_serv_sock(-1),-tp(NULL) {}

        //完成TCP服务器端socket的初始化，线程池初始化
		bool HttpServerInit(std::string &ip, int port) {
            _serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            if(_serv_sock < 0){
                LOG("sock error :%s\n", strerror(errno));
                return flase;
            }
            sockaddr_in lst_addr;
            lst_addr.sin_family = AF_INET:
            lst_addr.sin_port = htons(port);
            lst_addr.sin_addr.s_addr = inet_addr(ip.c_str());
            socket_t len = sizeof(sockaddr*)&lst_addr, len) < 0) {
                LOG("bind error :%s\n",strerror(errno));
                close(_serv_sock);
                return flase;
            }
            if(listen(_serv_sock, MAX_LISTEN) < 0) {
                LOG("listen error: %s\n",strerror(errno));
                close(_serv_sock);
                return false;
            }
            _tp = new ThreadPool(MAX_THREAD);
            if(_tp = NULL){
                LOG("thread pool malloc error!!\n");
                return false;
            }

            if (_tp->ThreadPollInit() == false){
                LOG("thread pool init error!!\n");
                return false;
            }
            return true;
        }
        //开始获取客户端连接--创建任务，任务入队
		bool Start() {
            while(1){
                sockaddr_in addr;
                socklen_t len = sizeof(sockaddr_in);
                int sock = accept(_ser_sock,(sockadd*)&addr,&len);
                if(sock < 0){
                    LOG("accept error: %s\n",strerror(errno));
                    continue;
                }
                HttpTask ht
                ht.SetHttpTask(sock ,HttpHandler);
                _tp->PushTask(ht);
            }
            return true;
        }
};

int main()
{
    return 0;
}



class RequestInfo
{
	//包含HttpRequest解析出的请求信息
	public:
		std::string _method;//请求方法
		std::string _version;//协议版本
		std::string _path_info;//资源路径
		std::string _path_phys;//资源实际路径
		std::string _query_string;//查询字符串
		std::unordered_map<std::string, std::string> _hdr_list;//整个头部信息的键值对
		struct stat _st;
    public:
        std::string _err_code;
    public:
        std::string _err_code;
    public:
        //判断请求类型
        bool RequestIsCGI() { 
            return true;
        }


};
class HttpRequest
{
	//http数据的接收接口
	//http数据的解析接口
	//对外提供能够获取处理结果的接口
	private:
		int _cli_sock;
		std::string &Header;
		RequestInfo _rep_info;
	public:
		HttpRequest(int sock):_cli_sock(sock){}
		//接收http请求头
        bool RecvHttpHeader(RequestInfo &info){
            char tmp[MAX_HTTPHDR];
            while(1){
                int recv(_cli_sock,buf ,MAX_HTTPHDR , MSG_PEEK);
                if(ret <= 0){
                    if(errno == EINTR || errno == EAGAIN) {

                    }
            }
            } 

            return true;
        }
        //解析http请求头
		bool ParseHttpHeader(){

        }
		RequestInfo & GetRequestInfo();//向外提供解析结果
};
class HttpResponse
{
	//文件请求（完成文件下载/列表功能）接口
	//CGI请求接口
private:
	int _cli_sock;
	std::string _etag;
	std::string _mtime;
	std::string _cont_len;
public:
    HttpResponse(int sock):_cli_sock(sock) {}
	bool InitResponse(RequestInfo req_info);//初始化一些请求的响应信息
	bool ProcessFile(RequestInfo &file);//文件下载列表
	bool ProcessList(RequestInfo &file);//文件列表功能
	bool ProcessCGI(RequestInfo &file);//cgi请求处理
    
    
    bool ErrHandler(RequestInfo &file);//处理错误响应
    bool CGIHandler(RequestInfo &file){
        InitResepose(info);//初始化cgi响应
        ProcessCGI(info);//执行cgi响应
    }
    bool Filehandler(RequestInfo &file)  {
        InitResponse(info);
        if(DIR){
            ProcessList(info);//执行文件列表展示响应
        }else{
            ProcessFile(info);//执行文件下载
        }
    }
};
class UpLoad
{
	//CGI外部程序中的文件上传功能处理接口
private:
	int _file_fd
	bool _is_store_file;
	std::string _content_length;
	std::string _file_name;
	std::string _first_boundry;
	std::string _middle_voundry;
	std::string _last_boundry;
public:
	bool InitUpLoadInfo();//初始化上传文件的信息 正文长度；
	bool ProessUpLoad();//完成文件的上传存储功能
};
class Utils
{
	//提供一些公用的接口
};