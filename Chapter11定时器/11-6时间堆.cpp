#ifndef MIN_HEAP
#define MIN_HEAP

#include <iostream>
#include <netinet/in.h>
#include <time.h>
using std::exception;

#define BUFFER_SIZE 64
class heap_timer;   //前向声明
struct client_data{
    sockaddr_in address;
    int sockfd;
    char buf[BUFFER_SIZE];
    heap_timer* timer;
};

//定时器类
class heap_timer{
    public:
    heap_timer(int delay){
        expire = time(nullptr) + delay;
    }
    public:
    time_t expire;  //定时器生效的绝对时间
    void(*cb_func)(client_data*);   //定时器的回调函数
    client_data* user_data;
};

//时间堆类
class time_heap{
    public:
    //构造函数之一，初始化一个大小为cap的空堆
    time_heap(int cap)throw(std::exception):capacity(cap), cur_size(0){
        array = new heap_timer*[capacity];  //创建堆数组
        if(!array){
            throw std::exception();
        }
        for(int i = 0; i < capacity; i++){
            array[i] = nullptr;
        }
    }
    //构造函数之二，用已有数组来初始化堆
    time_heap(heap_timer** init_array, int size, int capacity)throw(std::exception):cur_size(size), capacity(capacity){
        if(capacity < size){
            throw std::exception();
        }
        array = new heap_timer*[capacity];//创建堆数组
        if(!array){
            throw std::exception();
        }
        for(int i = 0; i < capacity; i++){
            array[i] = nullptr;
        }
        if(size != 0){
            //初始化堆数组
            for(int i = 0; i < size; i++){
                array[i] = init_array[i];
            }
            for(int i = (cur_size - 1)/2; i >= 0; --i){
                //对数组中的第[(cur_size-1)/2]~0个元素执行下沉操作
                percolate_down(i);
            }
        }
    }
    //销毁时间堆
    ~time_heap(){
        for(int i = 0; i < cur_size; i++){
            delete array[i];
        }
        delete[] array;
    }
    public:
    //添加目标定时器timer
    void add_timer(heap_timer* timer)throw(std::exception){

    }
    //删除目标定时器timer
    void del_timer(heap_timer* timer){
        
    }
    //删除堆顶部的定时器
    heap_timer* top()const{

    }
    //删除堆顶部的定时器
    void pop_timer(){

    }
    //心博函数
    void tick(){

    }
    bool empty()const{return cur_size == 0;}
    private:
    //最小堆的下沉操作，它确保数组中以第hole个节点作为根的子树拥有最小对性质
    void percolate_down(int hole){

    }
    //将堆数组容量扩大1倍
    void resize()throw(std::exception){
        
    }
    private:
    heap_timer** array; //堆数组
    int capacity;       //堆数组的容量
    int cur_size;       //堆数组当前包含元素的个数
};

#endif