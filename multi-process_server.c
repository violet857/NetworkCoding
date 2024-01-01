/* Use fork/waitpid to modify tcp server as a multi-process concurrent server, 
which can handle multi-client request concurrently. 
client端就是文件夹中的tcp_client.c      */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>

// 子进程中运行的，处理客户端请求的函数
int HandleTCPClient(int clnt_sock);

int main(){
    int serv_sock;
    struct sockaddr_in serv_addr;
    int clnt_sock;
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_size;
    unsigned int childCount = 0;
    pid_t processID;

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
        if (clnt_sock == -1){
            printf("Error occured! \n");
            exit(1);
        }
        processID = fork();
        if (processID < 0){
            printf("Error occured! \n");
            exit(1);
        }else if (processID == 0){  // 子进程
            close(serv_sock);  // 子进程关闭listen socket
            HandleTCPClient(clnt_sock);
            exit(0);
        }
        printf("with child process: %d\n", (int)processID);
        close(clnt_sock);  // 父进程关闭客户端的accpet socket
        childCount++;
        while (childCount){
            processID = waitpid((pid_t)-1, NULL, WNOHANG);  // 父进程以非阻塞模式等待任意一个子进程结束
            if (processID < 0){
                printf("Error occured! \n");
                exit(1);
            }else if (processID == 0){
                break;
            }else{
                childCount--;
            }
        }
    }
    return 0;
}

int HandleTCPClient(int clnt_sock){
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