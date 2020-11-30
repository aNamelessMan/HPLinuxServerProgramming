#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/uio.h>
#include <memory>

#define BUFFER_SIZE 1024

static const char* status_line[2] = {"200 OK", "500 Internal server error"};
/*用例
服务端：    ./bin/6-2Web服务器上的集中写 localhost 54321 ./Chapter6高级IO函数/6-1CGI服务器原理.cpp
客户端：    telnet localhost 54321
实测当传入的文件不存在时，程序会在delete处崩溃
解决方法很简单，定义file_buf时初始化为nullptr，删除之前先判断是否为空
*/
int main(int argc, char* argv[]){
    if(argc <= 3){
        printf("usage: %s ip_address port_number filename\n", basename(argv[0]));
        return 1;
    }
    const char *ip = argv[1];
    int port = atoi(argv[2]);
    const char *file_name = argv[3];

    struct sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &address.sin_addr);
    address.sin_port = htons(port);

    int sock = socket(PF_INET, SOCK_STREAM, 0);
    assert(sock >= 0);

    //设置让socket地址可以立即被重用，这样可以在短时间内多次试验
    int reuse = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

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
        //用于保存HTTP应答的状态行，头部字段和一个空行的缓存区
        char header_buf[BUFFER_SIZE];
        memset(header_buf, '\0', BUFFER_SIZE);
        //用于存放目标文件内容的应用程序缓存
        char *file_buf = nullptr;
        //用于获取目标文件的属性，比如是否为目录，文件大小等
        struct stat file_stat;
        //记录目标文件是否是有效文件
        bool vaild = true;
        //缓存区header_buf目前已经使用了多少字节的空间
        int len = 0;
        if(stat(file_name, &file_stat) < 0){//目标文件不存在
            vaild = false;
        }else{
            if(S_ISDIR(file_stat.st_mode)){//目标文件是一个目录
                vaild = false;
            }else if(file_stat.st_mode & S_IROTH){//当前用户有读取目标文件的权限
                //动态分配缓存区file_buf，并指定其大小为目标文件的大小file_stat.st_size + 1，然后将目标文件读入缓存区file_buf
                int fd = open(file_name, O_RDONLY);
                file_buf = new char[file_stat.st_size + 1];

                memset(file_buf, '\0', file_stat.st_size + 1);
                if(read(fd, file_buf, file_stat.st_size) < 0){
                    vaild = false;
                }
            }else{
                vaild = false;
            }
        }
        //如果目标文件有效，则发送正常的HTTP应答
        if(vaild){
            //将HTTP应答的状态行、“Content-length”头部字段和一个空白行依次加入header_buf中
            ret = snprintf(header_buf, BUFFER_SIZE - 1, "%s %s\r\n", "HTTP/1.1", status_line[0]);
            len += ret;
            ret = snprintf(header_buf + len, BUFFER_SIZE - 1 - len, "Content-Length: %ld\r\n", file_stat.st_size);
            len += ret;
            ret = snprintf(header_buf + len, BUFFER_SIZE - 1 - len, "%s", "\r\n");

            //利用writev将header_buf和file_buf的内容一并写出
            struct iovec iv[2];
            iv[0].iov_base = header_buf;
            iv[0].iov_len = strlen(header_buf);
            iv[1].iov_base = file_buf;
            iv[1].iov_len = file_stat.st_size;
            ret = writev(connfd, iv, 2); 
        }else{//目标文件无效，告知客户端发生了“内部错误”
            ret = snprintf(header_buf, BUFFER_SIZE - 1, "%s %s\r\n", "HTTP/1.1", status_line[1]);
            len += ret;
            ret = snprintf(header_buf + len, BUFFER_SIZE - 1 - len, "%s", "\r\n");
            send(connfd, header_buf, strlen(header_buf), 0);
        }
        close(connfd);
        if(file_buf)
            delete[] file_buf;
    }
    close(sock);
    return 0;
}