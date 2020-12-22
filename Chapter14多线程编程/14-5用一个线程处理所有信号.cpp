#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

#define handle_error_en(en, msg) do{errno = en; perror(msg); exit(EXIT_FAILURE);}while(0)

static void *sig_thread(void *arg){
    sigset_t *set = (sigset_t *)arg;
    int s, sig;
    for( ;; ){
        //第二个步骤，调用sigwait等待信号
        s = sigwait(set, &sig);
        if(s != 0)
            handle_error_en(s, "sigwait");
        printf("Signal handling thread got signal %d\n", sig);
    }
}

int main(int argc, char* argv[]){
    pthread_t thread;
    sigset_t set;
    int s;

    //第一个步骤，在主线程中设置信号掩码
    sigemptyset(&set);
    sigaddset(&set, SIGQUIT);
    sigaddset(&set, SIGUSR1);
    s = pthread_sigmask(SIG_BLOCK, &set, nullptr);
/*
#include <pthread.h>
#include <signal.h>
int pthread_sigmask(int how, const sigset_t* newmask, sigset_t* oldmask);
用于设置线程的信号掩码

进程中的所有线程共享该进程的信号，线程库根据线程掩码决定将信号发送给那个具体的线程，因此如果我们在每个子线程中都单独设置信号掩码，会容易混淆出错
所有线程共享信号处理函数，也就是说，当我们在一个线程中设置了某个信号的信号处理函数后，它将覆盖其他线程为同一个信号设置的信号处理函数

应该单独定义一个专门的线程来处理所有的信号：
1.在主线程创建其他子线程前调用pthread_sigmask来设置信号掩码，这样所有新建的子线程都将自动继承这个信号掩码，所有线程都不会响应被屏蔽的信号
2.在某个线程中调用以下函数来等待信号并处理之：
    #include <signal.g>
    int sigwait(const sigset_t* set, int* sig);
*/
    if(s != 0)
        handle_error_en(s, "pthread_sigmask");

    s = pthread_create(&thread, nullptr, &sig_thread, (void*)&set);
    if(s != 0)
        handle_error_en(s, "pthread_create");
    
    pause();
}