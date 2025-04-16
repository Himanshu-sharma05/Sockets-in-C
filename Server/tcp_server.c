#include<stdio.h>
#include<stdlib.h>

#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<string.h>
#include<stdbool.h>
#include<pthread.h>


// void startAcceptingIncomingConnections(int serverSocketFD){
//     pthread_t id;
//     pthread_create(&id,NULL,acceptNewConnectionRecieveAndPrintItsData(),serverSocketFD);
// }

// void acceptNewConnectionRecieveAndPrintItsData(int serverSocketFD){
//     while(true){
//         struct AcceptedSocket* clientSocket = acceptIncomingConnection(serverSocketFD);
//         recieveAndPrintAcceptedDataOnSeperateThread(clientSocket());
//     }
// }

// void recieveAndPrintAcceptedDataOnSeperateThread(struct AcceptedSocket *pSocket){
//     pthread_t id;
//     pthread_create(&id,NULL,recieveAndPrintIncomingData,pSocket->acceptedSocketFD)
// }

// void recieveAndPrintIncomingData(int socketFD){
//     char buffer[1024];
//     while(true){
//         ssize_t amaountRecieved = recv(socketFD,buffer,1024,0);

//     }
// }

struct AcceptedSocket * acceptIncomingConnection(int serverSocketFD);

struct AcceptedSocket
{
    int acceptedSocketFD;
    struct sockaddr_in address;
    int error;
    bool acceptedSuccessfully;
};

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

    // struct sockaddr_in clientAddress;
    // int clientAddressSize = sizeof(clientAddress);
    // int clientSocketFD = accept(serverSocketFD,(struct sockaddr *)&clientAddress,&clientAddressSize);
    struct AcceptedSocket* clientSocket = acceptIncomingConnection(serverSocketFD);


    char buffer[1024];
    while(true){
        ssize_t amaountRecieved = recv(clientSocket->acceptedSocketFD,buffer,1024,0);
        if(amaountRecieved > 0){
            buffer[amaountRecieved] = 0;
            printf("The recieved response is :%s",buffer);
        }
        if(amaountRecieved == 0){
            break;
        }
        
    }


    
    close(clientSocket->acceptedSocketFD);
    shutdown(serverSocketFD,SHUT_RDWR);

    return 0;
}

struct AcceptedSocket * acceptIncomingConnection(int serverSocketFD){
    struct sockaddr_in clientAddress;
    int clientAddressSize = sizeof(struct sockaddr_in);
    int clientSocketFD = accept(serverSocketFD,(struct sockaddr *)&clientAddress,&clientAddressSize);

    struct AcceptedSocket * acceptedSocket = malloc(sizeof(struct AcceptedSocket));
    acceptedSocket->address = clientAddress;
    acceptedSocket->acceptedSocketFD = clientSocketFD;
    acceptedSocket->acceptedSuccessfully = clientSocketFD>0;

    if(!acceptedSocket->acceptedSuccessfully){
        acceptedSocket->error = clientSocketFD;
    }


    return acceptedSocket;
}
