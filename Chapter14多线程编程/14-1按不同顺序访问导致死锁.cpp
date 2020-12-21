/*线程API
#include <pthread.h>
1.
int pthread_create(pthread_t* thread, const pthread_attr_t* attr, void* (*start_routine)(void*), void* arg);    用于创建一个线程

thread参数是新线程的标识符，后续 pthread_* 函数通过它来引用新线程。 其类型pthread_t定义如下：   typedef unsigned long int pthread_t
attr参数用于设置新线程的属性，传入nullptr表示使用默认线程属性
start_routine和arg属性分别指定新线程将允许的函数及其参数

成功时返回0，失败时返回错误码
2.
void pthread_exit(void* retval);
线程函数在结束时最好调用这个函数，以确保安全、干净的退出，它通过retval参数向线程的回首这传递其退出信息，它执行完后不会返回到调用者且永远不会失败
3.
int pthread_join(pthread_t thread, void** retval);
一个进程中的所有线程都可以调用pthread_join函数来回收其他线程(前提是目标线程是可回收的，见后文)即等待其他线程结束，这类似于回收进程的wait和waitpid系统调用。

thread参数是目标线程的标识符，retval参数则是目标线程返回的退出信息。该函数会一直阻塞，直到被回收的线程结束为止。成功时返回0，失败时返回错误码
4.
int pthread_cancel(pthread_t thread);
该函数用于取消线程

thread参数是目标线程的标识符。成功时返回0，失败时则返回错误码。
接收到取消请求的目标线程可以决定是否允许被取消以及如何取消
int pthread_setcancelstate(int state, int *oldstate);
int pthread_setcanceltype(int type, int *oldtype);
这两个函数的第一个参数分别用于设置线程的取消状态(指是否允许取消)和取消类型(指如何取消)，第二个参数则分别记录线程原来的取消状态和取消类型
*/

/*3种专门用于线程同步的机制：POSIX信号量、互斥量和条件变量
常用的5个POSIX信号量函数
#include <semaphore.h>
int sem_init(sem_t* sem, sem_t* sem, int pshared, unsigned int value);
int sem_destory(sem_t* sem);
int sem_wait(sem_t* sem);
int sem_trywait(sem_t* sem);
int sem_post(sem_t* sem);
这些函数的第一个参数sem指向被操作的信号量

sem_init函数用于初始化一个未命名的信号量
pshared参数指定信号量的类型。如果值为0就表示这个信号量是当前进程的局部信号量，否则该信号量就可以在多个进程之间共享，value参数指定信号量的初始值。
此外，初始化一个已经被初始化的信号量将导致不可预期的结果

sem_destory函数用于销毁信号量，以释放其占用的内核资源。如果销毁一个正在被其他线程等待的变量，则将导致不可预期的结果
sem_wait函数以原子操作的方式将信号量的值减1。如果信号量的值为0，则调用会被阻塞，直到这个信号量具有非0值
sem_trywait与sem_wait类似，不过它始终立即返回，相当于sem_wait的非阻塞版本。当信号量非0时，sem_trywait对信号量执行减1操作。当信号值为0时，它将返回-1并设置errno为EAGAIN。
sem_post函数以原子操作的方式将信号量的值加1。当信号量的值大于0时，其他正在调用sem_wait等待信号量的线程将被唤醒。
上面这些函数成功时返回0。失败时则返回-1并设置errno
*/

/*互斥锁    用于保护关键代码段，以确保其独占式的访问    类似于二进制信号量
互斥锁基础API
#include <pthread.h>
int pthread_mutex_init(pthread_mutex_t* mutex,const pthread_mutexattr_t* mutexattr);
int pthread_mutex_destory(pthread_mutex_t* mutex);
int pthread_mutex_lock(pthread_mutex_t* mutex);
int pthread_mutex_trylock(pthread_mutex_t* mutex);
int pthread_mutex_unlock(pthread_mutex_t* mutex);

这些函数的第一个参数mutex指向要操作的目标互斥锁，含义类似于信号量
*/

/*条件变量
如果说互斥锁是用于  同步线程对共享数据的访问
那么条件变量是用于  线程之间同步共享数据的值
条件变量提供了一种机制：当某个共享数据到达某个值的时候，唤醒等待这个共享数据的线程
int pthread_cond_init(pthread_cond_t* cond, const pthread_condattr_t cond_attr);
int pthread_cond_destory(pthread_cond_t* cond);
int pthread_cond_broadcast(pthread_cond_t* cond);   唤醒所有等待目标变量的线程
int pthread_cond_signal(pthread_cond_t* cond);      唤醒一个等待目标变量的线程
int pthread_cond_wait(pthread_cond_t* cond， pthread_mutex_t* mutex);
*/
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>

int main(){
    pthread_mutex_t locka;
    pthread_mutex_init(&locka, nullptr);

    pthread_mutex_lock(&locka);
    pthread_mutex_lock(&locka);//在同个线程中将普通锁枷锁之后，再次加锁会导致死锁

    return 0;
}