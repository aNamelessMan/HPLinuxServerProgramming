/*
poll系统调用
#include <poll.h>
int poll(struct pollfd fdarray[], nfds_t nfds, int timeout);

struct pollfd{
    int fd;         //文件描述符
    short events;   //注册的事件
    short revents;  //实际发生的事件，由内核填充
}

epoll系列系统调用
#include <sys/epoll.h>
int epoll_create(int size)
//传入size告诉内核事件表需要多大，返回一个用来标识内核中的该事件表的文件描述符，用于作为其他所有epoll函数的第一个参数

int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
//fd是要指定的文件描述符，op是操作类型（注册EPOLL_CTL_ADD/修改EPOLL_CTL_MOD/删除EPOLL_CTL_DEl）

int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);
*/