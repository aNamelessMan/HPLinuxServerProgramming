/*  select系统调用
#include <select.h>
int select(int fds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
参数含义为： 被监听的文件描述符总数，通常被设置为select监听的所有文件描述符中的最大值加1，因为文件描述符是从0开始的
fdset结构体包含一个long int数组，每一位表示一个文件描述符，应该用以下宏来操作
    FD_ZERO(fd_set *fdset)              清空所有位 
    FD_SET(int fd, fd_set *fdset)       设置某位  
    FD_CLR(int fd, fd_set *fdset)       清除某位    
    FD_ISSET(int fd, fd_set *fdset)     测试某位是否被设置
timeout结构体包含两个long   表示s和ms
设置最长等待时间
    空指针  无限等待直到某个文件描述符就绪
    0，0   立即返回
    other  最长等待时间
返回时修改timeout指向的值为等待时长，注意调用失败时此值是不确定的
调用成功是返回就绪的文件描述符的总数    失败时返回-1并设置errno
如果在等待期间程序收到信号，select立即返回-1，并直errno为EINTR
*/
/*
./bin/9-1同时接受普通数据和带外数据 localhost 54321
./bin/5-6发送带外数据 localhost 54321
输出如下：
get 5 bytes of normal data: 123ab
get 1 bytes of oob data: c
get 3 bytes of normal data: 123
*/
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>

int main(int argc, char* argv[]){
    if(argc <= 2){
        printf("usage: %s ip_address port_number\n", basename(argv[0]));
        return 1;
    }
    const char *ip = argv[1];
    int port = atoi(argv[2]);

    int ret = 0;
    struct sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &address.sin_addr);
    address.sin_port = htons(port);

    int listenfd = socket(PF_INET, SOCK_STREAM, 0);
    assert(listenfd >= 0);

    ret = bind(listenfd, (struct sockaddr*)&address, sizeof(address));
    assert(ret != -1);

    ret = listen(listenfd, 5);
    assert(ret != -1);

    struct sockaddr_in client;
    socklen_t client_addrlength = sizeof(client);
    int connfd = accept(listenfd, (struct sockaddr*)&client, &client_addrlength);

    if(connfd < 0){
        printf("errno is: %d\n", errno);
        close(listenfd);
        return 0;
    }

    char buf[1024];
    fd_set read_fds;
    fd_set exception_fds;
    FD_ZERO(&read_fds);
    FD_ZERO(&exception_fds);

    while(1){
        //每次调用select前都要重新再设置read_fds和exception_fds中的文件描述符，因为每次事件发生后，文件描述符集合都会被内核修改
        FD_SET(connfd, &read_fds);
        FD_SET(connfd, &exception_fds);
        ret = select(connfd + 1, &read_fds, nullptr, &exception_fds, nullptr);
        if(ret < 0){
            printf("seletion failure\n");
            break;
        }
        //如果是因为connfd可读而返回，调用recv函数读取数据  flag为0
        if(FD_ISSET(connfd, &read_fds)){
            memset(buf, '\0', sizeof(buf));
            ret = recv(connfd, buf, sizeof(buf) - 1, 0);//留一位作为C风格字符串的结束标志
            if(ret <= 0)break;
            printf("get %d bytes of normal data: %s\n", ret, buf);
        }
        if(FD_ISSET(connfd, &exception_fds)){
            //如果是因为connfd接收到带外数据，即异常        flag为MSG_OOb
            memset(buf, '\0', sizeof(buf));
            ret = recv(connfd, buf, sizeof(buf) - 1, MSG_OOB);
            if(ret <= 0)break;
            printf("get %d bytes of oob data: %s\n", ret, buf);
        }
    }
    close(connfd);
    close(listenfd);
    return 0;
}