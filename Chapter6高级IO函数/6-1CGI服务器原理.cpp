/*
#include <unistd.h>
int dup(int file_descriptor);   打开一个新的文件描述符，该描述符与参数描述符指向相同的文件/管道/网络连接
int dup2(int file_descriptor_one, int file_descriptor_two);
*/

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

int main(int argc, char* argv[]){
    if(argc <= 2){
        printf("usage: %s ip_address port_number\n", basename(argv[0]));
        return 1;
    }
    const char* ip = argv[1];
    int port = atoi(argv[2]);

    struct sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &address.sin_addr);
    address.sin_port = htons(port);

    int sock = socket(PF_INET, SOCK_STREAM, 0);
    assert(sock >= 0);

    int ret = bind(sock, (struct sockaddr*)&address, sizeof(address));
    assert(ret != -1);

    ret = listen(sock, 5);
    assert(ret != -1);

    struct sockaddr_in client;
    socklen_t client_addrlength = sizeof(client);
    int connfd = accept(sock, (struct sockaddr*)&client, &client_addrlength);

    if(connfd < 0){
        printf("errno is: %d\n", errno);
    }else{
        close(STDOUT_FILENO);
        dup(connfd);//打开新的描述符(一定是标准输出)，所以现在标准输出指向网络连接socket
        printf("abcd\n");
        close(connfd);
    }

    close(sock);
    return 0;
}