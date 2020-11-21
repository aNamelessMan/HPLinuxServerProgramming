/*                                  1.socket地址API
    通用socket地址
#include <bits/socket.h>
struct sockaddr{
    sa_family_t sa_family;
    char sa_char[14];
}

    IPv4专用socket地址结构体
#include <arpa/inet.h>
struct sockaddr_in{             socket地址包括  地址族、端口号、IP
    sa_family_t sin_family;     AF_INET
    u_int16_t sin_port;         网络序
    struct in_addr sin_addr;
}
struct in_addr{                 IPv4地址结构体
    u_int32_t s_addr;           要用网络序(大端)
}
*/

/*
    IP地址转换函数
点分十进制字符串表示IPv4地址 与 网络字节序整数 表示之间的转化
#include <arpa/inet.h>
in_addr_t inet_addr(const char* strptr);        失败时返回INADDR_NONE
int inet_aton(const char* cp, struct in_addt* inp);     结构存储于inp指向的结构，成功返回1，失败返回0
char* inet_ntoa(struct in_addr in); 该函数用一个内部静态变量存储转化结果，返回值指向该静态内存，因此是不可重入

点分十进制字符串表示IPv4地址/十六进制字符串表示的IPv6地址 与 网络字节序整数 表示之间的转化
#include <arpa/inet.h>
int inet_pton(int af, const char* src, void* dst);      af为地址族:AF_INET/AF_INET6 src为要转化的地址字符串 dst是要存放的位置   成功返回1失败返回0并设置errno
const char* inet_ntop(int af, const void* src, char* dst, socklen_t cnt);   cnt是目标存储单元大小，可以用以下两个宏来指定

#include <netinet/in.h>
#define INET_ADDRSTRLEN 16
#define INET6_ADDRSTRLEN 46
*/

/*                                  2.创建socket
#include <sys/types.h>
#include <sys/socket.h>
int socket(int domain, int type, int protocol); 

domain  为协议族(TCP/IP协议族为PF_INET/PF_INET6)
type    为服务类型(TCP为SOCK_STREAM UDP为SOCK_UGRAM)    还可以和SOCK_NONBLOCK和SOCK_CLOEXEC相与
protocol    选择具体的协议，一般情况下是唯一的，因此设为0表示选择默认协议即可
成功返回socket文件描述符，失败则返回-1并设置errno
*/

/*                                  3.命名socket    即将一个socket与socket地址绑定
#include <sys/types.h>
#include <sys/socket.h>
int bind(int sockfd, const struct sockaddr* my_addr, socklen_t addrlen);
成功返回0，失败则返回-1并设置errno
*/

#include <arpa/inet.h>
#include <stdio.h>
int main(){
    struct in_addr ipn1, ipn2;
    inet_aton("1.2.3.4", &ipn1);
    inet_aton("10.194.71.60", &ipn2);

    char* szValue1 = inet_ntoa(ipn1);
    char* szValue2 = inet_ntoa(ipn2);

    printf("%p\n", szValue1);
    printf("%p\n", szValue2);

    printf("%s\n", szValue1);
    printf("%s\n", szValue2);
}