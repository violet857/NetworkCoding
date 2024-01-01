//client端就是文件夹中的tcp_client.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>


int main(int argc, char* argv[]){
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_addr, clnt_addr;
    socklen_t clnt_addr_size;
    char message[] = "Get message";
    char recv_message[50];
    int check_fd;
    
    memset(&serv_addr, 0, sizeof(serv_addr)); // init
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // ip
    serv_addr.sin_family = AF_INET; // ipv4
    serv_addr.sin_port = htons(1234); // port

    serv_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (serv_sock == -1){
        printf("Error occured!2 \n");
        exit(1);
    }
    printf("Socket id: %d \n", serv_sock);
    if (bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1){
        printf("Error occured!3 \n");
        exit(1);
    }
    printf("Bind success! \n");
    if (listen(serv_sock, 5) == -1){ // listen socket, max 5 client
        printf("Error occured!4 \n");
        exit(1);
    }

    clnt_addr_size = sizeof(clnt_addr);
    fd_set reads, cpy_reads;  //定义select监听的文件描述符集合
    struct timeval timeout;  //定义select超时时间
    int fd_max,result,str_len;  //定义最大文件描述符，select返回值，读取的字符串长度
    FD_ZERO(&reads);  //初始化文件描述符集合
    FD_SET(serv_sock, &reads); // 将listen socket加入到监听集合中
    fd_max=serv_sock; //初始化监听集合中最大的文件描述符，此时集合中只有一个描述符
    while(1){
        cpy_reads = reads;  // 拷贝reads，因为在执行select之后会改变reads的值
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;  // 毫秒
        if((result = select(fd_max+1, &cpy_reads, 0, 0, &timeout)) == -1){  //检查是否有活跃的文件描述符
            break;  // select出错
        }else if(result == 0){
            continue;  // 超时，没有可读取的文件描述符
        }
        for (check_fd = 0; check_fd < fd_max+1; check_fd++){ // 遍历所有文件描述符
            if (FD_ISSET(check_fd, &cpy_reads)){  //判断文件描述符是否在监听集合中
                if(check_fd==serv_sock){  // 如果listen socket处于活跃状态，说明有新的client连接
                    clnt_addr_size = sizeof(clnt_addr);
                    clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
                    FD_SET(clnt_sock, &reads);  //将新连接的client加入到监听集合
                    if(fd_max < clnt_sock){ //如果大于监听的最大文件描述符
                        fd_max = clnt_sock; //更新最大文件描述符
                    }
                    printf("Connected client: %d\n", clnt_sock);
                }else{
                    str_len = read(check_fd, recv_message, sizeof(recv_message)); //读取client发来的可读数据
                    if(strcmp(recv_message, "q") == 0){ // client主动断开连接
                        FD_CLR(check_fd, &reads);  //从监听集合中移除
                        close(check_fd);  //关闭此client的accept socket
                        printf("Closed client: %d\n", check_fd);
                        continue;
                    }
                    if(str_len == 0){  //等于0说明client断开了连接
                        FD_CLR(check_fd, &reads);  //从监听集合中移除
                        close(check_fd);  //关闭此client的accept socket
                        printf("Closed client: %d\n", check_fd);
                    }else{
                        printf("Message from client %d: %s\n", check_fd, recv_message);
                        if(write(check_fd, message, sizeof(message)) == -1){ //向client发送数据
                            printf("Error occured! \n");
                            exit(1);
                        }; 
                    }
                }
            }
        }
    }
} 