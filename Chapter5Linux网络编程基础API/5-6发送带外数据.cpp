/*          6.接受连接
#include <sys/types.h>
#include <sys/socket.h>
int connect(int sockfd, const struct sockaddr *serv_addr, socklen_t addrlen);
成功则返回0 失败则返回-1并设置errno
*/
/*          7.关闭连接
#include <unistd.h>
int close(int fd);
将fd的引用计数-1，直到计数为0时才真正关闭连接
多进程程序中一次fork系统调用默认将父进程中打开的socket引用计数+1

如果非要立即终止连接，而不是将引用计数减1，可以使用系统调用shutdown
#include <sys/socket.h>
int shutdown(int sockfd, int howto);
*/
/*          8.数据读写
#include <sys/types.h>
#include <sys/socket.h>
ssize_t recv(int sockfd, void *buf, size_t len, int flags);
成功时返回读取的数据长度
出错时会返回-1，并设置errno

注意返回0说明收到fin，即对方已经关闭连接    (类似于read读文件时读到文件结束符)
缓冲区无可用数据会返回-1，并设置errno

ssize_t send(int sockfd, const void *buf, size_t len, int flags);
成功时返回写入的数据长度
出错时会返回-1，并设置errno
*/
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
//这是发送方/客户端程序
int main(int argc, char* argv[]){
    if(argc <= 2){
        printf("usage: %s ip_address port_number\n", basename(argv[0]));
        return 1;
    }
    const char* ip = argv[1];
    int port = atoi(argv[2]);

    struct sockaddr_in server_address;
    bzero(&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &server_address.sin_addr);
    server_address.sin_port = htons(port);

    int sockfd = socket(PF_INET, SOCK_STREAM, 0);
    assert(sockfd >= 0);
    if(connect(sockfd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0){
        printf("connection failed\n");
    }else{
        const char *oob_data = "abc";
        const char *normal_data = "123";
        send(sockfd, normal_data, strlen(normal_data), 0);
        send(sockfd, oob_data, strlen(oob_data), MSG_OOB);
        send(sockfd, normal_data, strlen(normal_data), 0);
    }

    close(sockfd);
    return 0;
}