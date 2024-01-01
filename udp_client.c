#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>

int main(int argc, char* argv[]){
    int sock;
    struct sockaddr_in serv_addr;
    char send_message[50];
    char receive_message[50];
    char c;
    int i;

    if(argc != 3){
        argv[1] = "127.0.0.1";
        argv[2] = "1234";
    }

    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); // 创建IPv4 UDP 套接字
    if(sock == -1){
        printf("socket() error!");
        exit(1);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;  // IPv4
    serv_addr.sin_addr.s_addr=inet_addr(argv[1]); 
    serv_addr.sin_port = htons(atoi(argv[2])); // atoi: 字符串转整数

    // 设定默认的⽬标地址和端⼝
    if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1){
        printf("connect() error!");
        exit(1);
    };

    // 除了发送接收函数之外，与tcp_client十分相似
    while(1){
        printf("Input message(q to quit): ");
        scanf("%s", send_message);
        if(send(sock, send_message, sizeof(send_message), 0) == -1){
            printf("send() error!");
            exit(1);
        }
        if(strcmp(send_message, "q") == 0) break;
        if(recv(sock, receive_message, sizeof(receive_message), 0) == -1){
            printf("recv() error!");
            exit(1);
        }
        printf("message from server: %s\n", receive_message);
    }
    serv_addr.sin_family = AF_UNSPEC;  // 值为0，不指定地址类型
    connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); //清空设置的默认目标地址
    close(sock); // 关闭套接字
    return 0;
}