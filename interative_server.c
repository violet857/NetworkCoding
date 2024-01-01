//client端是tcp_client.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>

int main(){
    int serv_sock;
    struct sockaddr_in serv_addr;
    int clnt_sock;
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_size;
    char message[]="Get message";
    char receive_message[50];
    // init
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET; 
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

    // 逐一接收客户端的连接请求
    clnt_addr_size = sizeof(clnt_addr);
    while (1){   
        clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size); // accept client
        if(clnt_sock == -1){
            printf("Error occured!5 \n");
            exit(1);
        }
        printf("New client connect success! \n");
        printf("Client IP: %s, port: %d\n", inet_ntoa(clnt_addr.sin_addr), ntohs(clnt_addr.sin_port)); 
        while(1){
            if(read(clnt_sock, receive_message, sizeof(receive_message)) == -1){  // read
                printf("Error occured!7 \n");
                exit(1);
            }
            if (strcmp(receive_message, "q") == 0){  // 如果客户端发送q，说明客户端要退出
                if(write(clnt_sock, "Quit!", sizeof("Quit!")) == -1){
                    printf("Error occured!13 \n");
                    exit(1);
                }
                printf("Client quit! Waiting for new client.\n");
                close(clnt_sock);
                memset(receive_message, 0, sizeof(receive_message));
                break;
            }else{
                printf("Message from client: %s\n", receive_message);  // 打印客户端发送的消息
                if(write(clnt_sock, message, sizeof(message)) == -1){  // write
                    printf("Error occured!14 \n");
                    exit(1);
                }
            }
        }
    }
    close(serv_sock);
    return 0;
}