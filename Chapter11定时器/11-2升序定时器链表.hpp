#ifndef LST_TIMER
#define LST_TIMER

#include <time.h>
#include <netinet/in.h>
#define BUFFER_SIZE 64
//前向声明
class util_timer;

//用户数据结构：客户端socket、socket文件描述符、读缓存和定时器
struct client_data{
    sockaddr_in address;
    int sockfd;
    char buf[BUFFER_SIZE];
    util_timer* timer;
};

//定时器类
class util_timer{
    public:
    util_timer():prev(nullptr), next(nullptr){}
    public:
    time_t expire;                  //任务的超时时间，这里使用绝对时间
    void (*cb_func)(client_data*);  //任务回调函数
    //回调函数处理的客户数据，有定时器的执行者传递给回调函数
    client_data* user_data;
    util_timer* prev;//指向上一个定时器
    util_timer* next;//指向下一个定时器
};

//定时器链表，它是一个升序、双向的链表，且带有头节点和尾节点
class sort_timer_lst{
    public:
    sort_timer_lst():head(nullptr), tail(nullptr){}
    //链表被销毁时，删除其中所有的定时器
    ~sort_timer_lst(){
        util_timer* tmp = head;
        while(tmp){
            head = tmp->next;
            delete tmp;
            tmp = head;
        }
    }

    //将目标定时器timer添加到链表中
    void add_timer(util_timer* timer){
        if(!timer){
            return;
        }
        if(!head){
            head = tail = timer;
            return;
        }
        /*如果目标定时器的超时时间小于当前链表中的所有定时器的超时时间，则把该定时器插入链表头部，作为链表新的头节点，
        否则就需要调用重载函数add_timer(util_timer* timer, util_timer* lst_head，把它插入链表中合适的位置，以保证链表的升序特性
        */
       if(timer->expire < head->expire){
           timer->next = head;
           head->prev = timer;
           head = timer;
           return;
       }
       add_timer(timer, head);
    }
    //当某个定时任务发生变化时，调整对应的定时器在链表中的位置。这个函数只考虑被调整的定时器的超时时间延长的情况，即该定时器需要往链表的尾部移动
    void adjust_timer(util_timer* timer){

    }
    //将目标定时器timer从链表中删除
    void del_timer(util_timer* timer){

    }
    //SIGALRM信号每次被触发就在其信号处理函数（如果使用统一事件源，则是主函数）中执行一次tick函数，已处理链表上到期的任务
    void tick(){

    }

    private:
    //一个重载的辅助函数，它被public中的add_timer和adjust_timer函数调用，该函数表示将目标定时器timer添加到节点lst_head之后的部分链表
    void add_timer(util_timer* timer, util_timer* lst_head){
        util_timer* prev = lst_head;
        util_timer* tmp = prev->next;
        //遍历lst_head节点之后的部分链表，直到找到一个超时时间大于目标定时器的超时时间的节点，并将目标定时器插入该节点之前
        while(tmp){
            if(timer->expire < tmp->expire){
                prev->next = timer;
                timer->next = tmp;
                tmp->prev = timer;
                timer->prev = prev;
                break;
            }
            prev = tmp;
            tmp = tmp->next;
        }
        //如果遍历完lst_head节点之后的部分链表，仍未找到超时时间大于目标定时器的超时时间的节点，则将目标定时器插入链表尾部，并将它设置为链表新的尾节点
        if(!tmp){
            prev->next = timer;
            timer->prev = prev;
            timer->next = nullptr;
            tail = timer;
        }
    }
    util_timer* head;
    util_timer* tail;
};

#endif