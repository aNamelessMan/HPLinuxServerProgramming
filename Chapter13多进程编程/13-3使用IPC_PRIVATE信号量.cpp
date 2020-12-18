#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

union semun{
    int val;                    //用于SETVAL命令
    struct semid_ds* buf;       //用于IPC_STAT和IPC_SET命令
    unsigned short int* array;  //用于GETALL和SETALL命令
    struct seminfo* __buf;      //用于IPC_INFO命令
};
//op为-1时执行p操作，op为1时执行v操作
void pv(int sem_id, int op){
    struct sembuf sem_b;
    sem_b.sem_num = 0;
    sem_b.sem_op = op;
    sem_b.sem_flg = SEM_UNDO;
    semop(sem_id, &sem_b, 1);
}
/*semop系统调用
#include <sys/sem.h>
int semop(int sem_id, struct sembuf* sem_ops, size_t num_sem_ops);
semop系统调用改变信号量的值，即执行P/V操作

sem_id参数是由semget调用返回的信号集标识符，用于指定被操作的目标信号量集
sem_ops参数指向一个sembuf结构体类型的数组，定义如下：
    struct sembuf{
        unsigned short int sem_num;     指定信号集  中  信号量的编号
        short int sem_op;               指定操作类型    可选值为正整数/0/负整数
        short int sem_flg;              影响操作的行为  可选值为IPC_NOWAIT/SEM_UNDO
    }
num_sem_ops参数指定要执行的操作个数，即sem_ops数组中元素的个数，semop对数组sem_ops中的每个成员按顺序依次执行操作，并且该过程是原子操作

成功时返回0，失败时则返回1并设置errno
*/

int main(int argc, char* argv[]){
    int sem_id = semget(IPC_PRIVATE, 1, 0666);
    // printf("%d\n", IPC_PRIVATE);
    // printf("%d\n", sem_id);
    //可以看到key和返回值并不相同，key用作semget参数，返回值用作semop，semctl参数

/*semget系统调用
#include <sys/sem.h>
int semget(key_t key, int num_sems, int sem_flags);
用于创建一个新的信号量集    或者    获取一个已经存在的信号量集

key参数用来标识一个全局唯一的信号量集，就像文件名全局唯一地标识一个文件一样，想通过信号量来通信的进程需要使用相同的键值来创建/获取该信号量，这个key只用于semget
num_sems参数指定要创建/获取的信号量集中信号量的个数，如果是创建，则该值必须被指定；如果是获取已经存在的信号量，则可以将它设置为0
sem_flags参数指定一组标志。

返回值是一个称为信号量标识符的整数，semop和semctl将使用它
*/
    union semun sem_un;
    sem_un.val = 1;
    semctl(sem_id, 0, SETVAL, sem_un);
/*semctl系统调用
#include <sys/sem.h>
int semctl(int sem_id, int sem_num, int command, ...);
semctl系统调用允许调用者对信号量进行直接控制

sem_id参数是由semget调用返回的信号量集标识符，用于指定被操作的信号量集
sem_num参数指定被操作的信号量在信号量集中的编号，command参数指定要执行的命令
有的命令需要调用者传递第4个参数，第4个参数的类型有用户自己定义，但sys/sem.h头文件给出了它的推荐格式，即上面的semun

成功时的返回值取决于command参数，失败时返回-1并设置errno
*/

    pid_t id = fork();
    if(id < 0){
        return 1;
    }else if(id == 0){
        printf("child try to get binary sem\n");
        //在父子进程间共享IPC_PRIVATE信号量的关键就在于二者都可以操作该信号量的标识符sem_id
        pv(sem_id, -1);
        printf("child get the sem and would release it after 5 seconds\n");
        sleep(5);
        pv(sem_id, 1);
        exit(0);
    }else{
        printf("parent try to get binary sem\n");
        pv(sem_id, -1);
        printf("parent get the sem and would release it after 5 seconds\n");
        sleep(5);
        pv(sem_id, 1);
    }

    waitpid(id, nullptr, 0);
    semctl(sem_id, 0, IPC_RMID, sem_un);//删除信号量
    return 0;
}