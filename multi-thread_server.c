// client端就是文件夹中的tcp_client.c      
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <pthread.h>

// 子线程运行的，处理客户端请求的函数
void *HandleTCPClient(void *arg);

int main(){
    int serv_sock;
    struct sockaddr_in serv_addr;
    int clnt_sock;
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_size;
    pthread_t pthreadID; 

    // init
    memset(&serv_addr, 0, sizeof(serv_addr)); 
    serv_addr.sin_family = AF_INET; // ipv4
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(1234); 

    serv_sock = socket(AF_INET, SOCK_STREAM, 0); // create socket, AF_INET: ipv4, SOCK_STREAM: TCP
    if (serv_sock == -1){
        printf("Error occured! \n");
        exit(1);
    }
    printf("Socket id: %d \n", serv_sock);

    // bind and listen
    if (bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1){
        printf("Error occured! \n");
        exit(1);
    }
    printf("Bind success! \n");
    if (listen(serv_sock, 5) == -1){
        printf("Error occured! \n");
        exit(1);
    }

    while (1){
        clnt_addr_size = sizeof(clnt_addr);
        clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
        if (clnt_sock == -1){  // accept失败
            printf("Error occured! \n");
            exit(1);
        }
        int *clnt_sock_copy = malloc(sizeof(int));  // 避免临界区问题
        if (clnt_sock_copy == NULL) {
            close(clnt_sock);  // 无法分配，server无法接受新的连接
            continue;
        }
        *clnt_sock_copy = clnt_sock;
        if (pthread_create(&pthreadID, NULL, HandleTCPClient, (void *)clnt_sock_copy) != 0){ 
            printf("Error occured! \n"); // 创建失败
            exit(1);
        }else{
            pthread_detach(pthreadID);  // 子线程分离，结束时自动回收资源
        }
        //清空pthreadID,防止影响下一次循环时创建线程
        memset(&pthreadID, 0, sizeof(pthreadID));
    }
    return 0;
}

void *HandleTCPClient(void *arg){
    int clnt_sock = *((int *)arg);
    free(arg);
    char receive_message[50];
    char message[]="Get message";
    while(1){
       if(read(clnt_sock, receive_message, sizeof(receive_message)) == -1){
            printf("Error occured! \n");
            exit(1);
        }
        if (strcmp(receive_message, "q") == 0){  // 如果客户端发送q，说明客户端要退出
            if(write(clnt_sock, "Quit!", sizeof("Quit!")) == -1){
                printf("Error occured! \n");
                exit(1);
            }
            printf("Client quit! \n");
            break;
        }else{
            printf("Message from client: %s\n", receive_message);  // 打印客户端发送的消息
            if(write(clnt_sock, message, sizeof(message)) == -1){  // write
                printf("Error occured! \n");
                exit(1);
            }
        }       
    }   
    close(clnt_sock);
    return 0;
}