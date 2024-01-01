// tcp client
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>


int main(int argc, char* argv[]){
    int sock; 
    struct sockaddr_in serv_addr;
    char message[50];  // 用于接收server端发送的信息
    char send_message[50];
    if(argc != 3){  // 检查参数个数，如果不是3个使用默认值
        argv[1] = "127.0.0.1";
        argv[2] = "1234";
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);  // 创建IPv4 TCP 套接字
    if(sock == -1){
        printf("socket() error!");
        exit(1);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));  //初始化网络地址结构 
    serv_addr.sin_family = AF_INET;  // 网络地址结构类型为IPv4
    serv_addr.sin_addr.s_addr=inet_addr(argv[1]); // 向结构中存储ip
    serv_addr.sin_port = htons(atoi(argv[2]));  // 向结构中存储port 

    if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1){ // 向server端发送连接请求
        printf("connect() error!");
        exit(1);
    }
    do{
        printf("Input message(q to quit): \n");
        scanf("%s", send_message);
        write(sock, send_message, sizeof(send_message));
        if (read(sock, message, sizeof(message)-1) == -1){  // 读取server端发送的信息
            printf("read() error!");
            exit(1);
        }
        printf("Message from server: %s\n", message);
    }while (strcmp(send_message, "q") != 0);
    printf("Quit!\n");
    close(sock);
    return 0;
}