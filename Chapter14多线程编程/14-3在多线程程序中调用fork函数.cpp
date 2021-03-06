#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <wait.h>

pthread_mutex_t mutex;

//14-4  使用pthread_atfork函数
void prepare(){
    pthread_mutex_lock(&mutex);
}

void infork(){
    pthread_mutex_unlock(&mutex);
}

//子线程类型的函数。它首先获得互斥锁mutex，然后暂停5s，再释放该互斥锁
void* another(void* arg){
    printf("in child thread, lock the mutex\n");
    pthread_mutex_lock(&mutex);
    sleep(5);
    pthread_mutex_unlock(&mutex);
    return nullptr;
}

int main(){
    pthread_mutex_init(&mutex, nullptr);
    pthread_t id;
    pthread_create(&id, nullptr, another, nullptr);
    //父进程中的主线程暂停1s，以确保在执行fork操作之前，子线程已经开始运行并获得了互斥量mutex
    sleep(1);

    pthread_atfork(prepare, infork, infork);
    /*
    #include <pthread.h>
    int pthread_atfork(void (*prepare)(void), void (*parent)(void), void(*child)(void));
    用于清理互斥锁的状态

    prepare在fork调用创建出子进程之前被执行，获取/等待/锁住 所有父进程中的互斥锁
    parent在fork返回之前，在父进程中被执行，释放所有在prepare中被锁住的互斥锁
    child在fork返回之前，在子进程中被执行，释放所有在prepare中被锁住的互斥锁

    因此fork之后子进程父进程中的锁都处于空闲状态
    */

    int pid = fork();
    if(pid <0){//出错
        pthread_join(id, nullptr);      //等待子线程结束
        pthread_mutex_destroy(&mutex);  //回收资源
        return 1;
    }else if(pid == 0){
        printf("I am in the child, want to get the lock\n");
        //子进程从父进程继承了互斥锁mutex的状态，该互斥锁处于锁住的状态，这是由父进程中的子线程执行pthread_mutex_lock引起的，
        //因此，下面这句枷锁操作会一直阻塞，尽管从逻辑上来说它是不应该阻塞的
        pthread_mutex_lock(&mutex);
        printf("I can not run to here, oop...\n");
        pthread_mutex_unlock(&mutex);
        exit(0);
    }else{
        wait(nullptr);
    }
    pthread_join(id, nullptr);
    pthread_mutex_destroy(&mutex);
    return 0;
}