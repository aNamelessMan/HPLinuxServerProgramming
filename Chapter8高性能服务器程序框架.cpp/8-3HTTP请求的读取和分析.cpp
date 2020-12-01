#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>

#define BUFFER_SIZE 4096    //缓冲区大小
//主状态机有两种可能状态：正在分析请求行，正在分析头部字段
enum CHECK_STATE{CHECK_STATE_REQUESTLINE, CHECK_STATE_HEADER};
//次状态机有三种可能状态，即行的读取状态：读取到一个完整行，行出错，行数据尚不完整
enum LINE_STATUS{LINE_OK, LINE_BAD, LINE_OPEN};
//对HTTP请求的处理结果，包括：NO_REQUEST表示请求不完整，需要继续读取客户端数据；GET_REQUEST表示获得了一个完整的客户请求；BAD_REQUEST表示客户请求有语法错误
//                        FORBIDDEN_REQUEST表示客户对资源没有足够的访问权限；INTERNAL_ERROR表示服务器内部错误；CLOSED_CONNECTION表示客户端已经关闭连接了
enum HTTP_CODE{NO_REQUEST, GET_REQUEST, BAD_REQUEST, FORBIDDEN_REQUEST, INTERNAL_ERROR, CLOSED_CONNECTION};

//为了简化，只是根据服务器处理结果发送如下成功或失败信息，而不是完整的HTTP应答报文
static const char* szret[] = {"I get a correct result\n", "Something wrong\n"};

//次状态机  用于解析一行的内容
LINE_STATUS prase_line(char *buffer, int &checked_index, int &read_index){
    char temp;
    //checked_index指向buffer（应用程序的读缓冲区）中当前正在分析的字节，read_index指向buffer中客户数据的尾部的下一字节，
    //buffer中第0~(checked_index-1)字节都已分析完毕，第checked_index~(read_index-1)字节由下面的循环挨个分析
    for( ; checked_index < read_index; ++checked_index){
        //获取当前要分析的字节
        temp = buffer[checked_index];
        //若当前字节是'\r'，即回车符，则说明说明可能读取到一个完整的行
        if(temp == '\r'){
            //如果'\r'碰巧是当前buffer中最后一个被读入的客户数据，那么这次分析没有读取到一个完整的行，返回LINE_OPEN来表示还需要继续读取客户数据来进一步分析
            if(checked_index + 1 == read_index)return LINE_OPEN;
            else if(buffer[checked_index + 1] == '\r'){
                buffer[checked_index ++] = '\0';
                buffer[checked_index ++] = '\0';
                return LINE_OK;
            }else
                return LINE_BAD;
        }else if(temp == '\n'){//如果下一个字符是'\n'即换行符，也说明可能读取到一个完整的行         这里应该是为了处理\r和\n分两次读入的情况
            if((checked_index > 1) && buffer[checked_index - 1] == '\r'){
                buffer[checked_index - 1] = '\0';
                buffer[checked_index ++] = '\0';
                return LINE_OK;
            }else
                return LINE_BAD;
        }
    }
    return LINE_OPEN;
}