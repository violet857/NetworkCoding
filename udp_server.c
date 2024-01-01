// client端就是文件夹中的udp_client.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>

int main(){
    int serv_sock;
    struct sockaddr_in serv_addr;
    int recv_len;
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_size = sizeof(clnt_addr);
    char receive_message[50];
    char send_message[]="Get message";

    memset(&serv_addr, 0, sizeof(serv_addr)); // init
    serv_addr.sin_family = AF_INET; // ipv4
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // ip
    serv_addr.sin_port = htons(1234); // port

    // create listen socket, AF_INET and PF_INET are the same which is ipv4
    serv_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); 
    if (serv_sock == -1){
        printf("Error occured!2 \n");
        exit(1);
    }
    printf("Socket id: %d \n", serv_sock);

    //  bind to the socket
    if (bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1){ //combine socket and ip
        printf("Error occured!3 \n");
        exit(1);
    }
    printf("Bind success! \n");

    while (1){   
        if ((recv_len = recvfrom(serv_sock, receive_message, sizeof(receive_message), 0, (struct sockaddr *)&clnt_addr, &clnt_addr_size)) < 0) {
            printf("Error occured! \n");  // 接收失败
            exit(1);
        }
        if(strcmp(receive_message, "q") == 0){
            printf("Client quit!\n");  // 客户端取消了默认的server地址，客户端结束
            continue;
        }
        printf("Message from client: %s\n", receive_message);
        if(sendto(serv_sock, send_message, sizeof(send_message), 0, (struct sockaddr *)&clnt_addr, sizeof(clnt_addr)) == -1){
            printf("sendto() error!");  // 发送失败
            exit(1);
        }
    }
    close(serv_sock);
    return 0;
}