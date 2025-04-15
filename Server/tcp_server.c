#include<stdio.h>
#include<stdlib.h>

#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<string.h>

int main(){
    int serverSocketFD = socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in serverAdd;
    serverAdd.sin_family = AF_INET;
    serverAdd.sin_addr.s_addr = INADDR_ANY;
    serverAdd.sin_port = htons(3000);

    int result = bind(serverSocketFD,(struct sockaddr *)&serverAdd,sizeof(serverAdd));
    if(result == 0){
        printf("socket succesfully bound\n");
    }

    int listenResult = listen(serverSocketFD,10);

    struct sockaddr_in clientAddress;
    int clientAddressSize = sizeof(clientAddress);
    int clientSocketFD = accept(serverSocketFD,(struct sockaddr *)&clientAddress,&clientAddressSize);

    char buffer[1024];
    recv(clientSocketFD,buffer,1024,0);
    printf("The recieved response is :%s",buffer);

    return 0;
}
