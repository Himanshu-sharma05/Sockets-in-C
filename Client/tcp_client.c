#include<stdio.h>
#include<stdlib.h>

#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<string.h>
#include<stdbool.h>

int main(){
    char* ip = "127.0.0.1";
    int socketFD = socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    inet_pton(AF_INET,ip, &address.sin_addr.s_addr);
    address.sin_port =  htons(3000);


    int result = connect(socketFD,(struct sockaddr *)&address,sizeof address);
    
    if(result == 0){
        printf("connection was successful\n");
    }else{
        printf("connection was not successful\n");
    }

    char *line = NULL;
    size_t lineSize = 0;
    printf("Type your message ...\n");

    while(true){
        ssize_t charCount = getline(&line,&lineSize,stdin);
        if(charCount > 0){
            if(strcmp(line,"exit\n") == 0){
                break;
            }
            ssize_t amountSent = send(socketFD,line,charCount,0);
        }
    }

    // char* message;
    // message = "GET \\ HTTP/1.1\r\nHost:google.com\r\n\r\n";
    // send(socketFD,message,strlen(message),0);
    // char buffer[1024];
    // recv(socketFD,buffer,1024,0);
    // printf("Response was the : %s\n",buffer);

    close(socketFD);
    return 0;
}



