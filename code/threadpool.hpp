#ifndef __M_TP_H__
#define __M_TP_H__
#include <iostream>
#include <queue>
#include <pthread.h>

#define MAX_THREAD  5
typedef bool (*handler_t)(int);
class ThreadTask
{
    private:
        int _sock;
        handler_t _handler;
    public:
        ThreadTask():_sock(-1), _handler(NULL) {}
        ThreadTask(int sock, handler_t handler) {
            _sock = sock;
            _handler = handler;
        }
        void TT_SetTask(int sock, handler_t handler) {
            _sock = sock;
            _handler = handler;
        }
        void TT_Run() {
            _handler(_sock);
        }
};
class ThreadPool
{
    private:
        int _thread_max;
        int _thread_cur;
        bool _tp_quit;
        std::queue<ThreadTask *> _task_queue;
        pthread_mutex_t _lock;
        pthread_cond_t _cond;
    private:
        void LockQueue() {
            pthread_mutex_lock(&_lock);
        }
        void UnLockQueue() {
            pthread_mutex_unlock(&_lock);
        }
        void WakeUpOne() {
            pthread_cond_signal(&_cond);
        }
        void WakeUpAll() {
            pthread_cond_broadcast(&_cond);
        }
        void ThreadQuit() {
            _thread_cur--;
            UnLockQueue();
            pthread_exit(NULL);
        }
        void ThreadWait(){
            if (_tp_quit) {
                ThreadQuit();
            }
            pthread_cond_wait(&_cond, &_lock);
        }
        bool IsEmpty() {
            return _task_queue.empty();
        }
        static void *thr_start(void *arg) {
            ThreadPool *tp = (ThreadPool*)arg;
            while(1) {
                tp->LockQueue();
                while(tp->IsEmpty()) {
                    tp->ThreadWait();
                }
                ThreadTask *tt;
                tp->TP_PopTask(&tt);
                tp->UnLockQueue();
                tt->TT_Run();
                delete tt;
            }
            return NULL;
        }
    public:
        ThreadPool(int max=MAX_THREAD):_thread_max(max), _thread_cur(0), _tp_quit(false) {
            pthread_mutex_init(&_lock, NULL);
            pthread_cond_init(&_cond, NULL);
        }
        ~ThreadPool() {
            pthread_mutex_destroy(&_lock);
            pthread_cond_destroy(&_cond);
        }
        bool TP_PoolInit() {
            pthread_t tid;
            for (int i = 0; i < _thread_max; i++) {
                int ret = pthread_create(&tid, NULL, thr_start, this);
                if (ret != 0) {
                    std::cout<<"create pool thread error\n";
                    return false;
                }
            }
            return true;
        }
        bool TP_PushTask(ThreadTask *tt) {
            LockQueue();
            if (_tp_quit) {
                UnLockQueue();
                return false;
            }
            _task_queue.push(tt);
            WakeUpOne();
            UnLockQueue();
            return true;
        }
        bool TP_PopTask(ThreadTask **tt) {
            *tt = _task_queue.front();
            _task_queue.pop();
            return true;
        }
        bool TP_Quit() {
            LockQueue();
            _tp_quit = true;
            UnLockQueue();
            while(_thread_cur > 0) {
                WakeUpAll();
                usleep(1000);
            }
            return true;
        }
};

#endif
