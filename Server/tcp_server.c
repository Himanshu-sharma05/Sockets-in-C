#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>

struct AcceptedSocket {
    int acceptedSocketFD;
    struct sockaddr_in address;
    int error;
    bool acceptedSuccessfully;
};

struct AcceptedSocket * acceptIncomingConnection(int serverSocketFD);
void * recieveAndPrintIncomingData(void *arg);
void startAcceptingIncomingConnections(int serverSocketFD);
void recieveAndPrintAcceptedDataOnSeperateThread(struct AcceptedSocket *pSocket);

void *recieveAndPrintIncomingData(void *arg) {
    int socketFD = *((int *)arg); 
    free(arg);

    char buffer[1024];
    while (true) {
        ssize_t amountRecieved = recv(socketFD, buffer, sizeof(buffer) - 1, 0);
        if (amountRecieved > 0) {
            buffer[amountRecieved] = 0;
            printf("The received response is: %s\n", buffer);
        }
        if (amountRecieved == 0) {
            break;
        }
    }

    close(socketFD); 
    return NULL;
}

void startAcceptingIncomingConnections(int serverSocketFD) {
    while (true) {
        struct AcceptedSocket *clientSocket = acceptIncomingConnection(serverSocketFD);
        if (clientSocket && clientSocket->acceptedSuccessfully) {
            recieveAndPrintAcceptedDataOnSeperateThread(clientSocket);
        }
    }
}

void recieveAndPrintAcceptedDataOnSeperateThread(struct AcceptedSocket *pSocket) {
    pthread_t id;
    int *fd_ptr = malloc(sizeof(int)); 
    if (fd_ptr) {
        *fd_ptr = pSocket->acceptedSocketFD;
        pthread_create(&id, NULL, recieveAndPrintIncomingData, fd_ptr);
    }
    free(pSocket);
}

struct AcceptedSocket * acceptIncomingConnection(int serverSocketFD) {
    struct sockaddr_in clientAddress;
    socklen_t clientAddressSize = sizeof(clientAddress);
    int clientSocketFD = accept(serverSocketFD, (struct sockaddr *)&clientAddress, &clientAddressSize);

    struct AcceptedSocket *acceptedSocket = malloc(sizeof(struct AcceptedSocket));
    if (!acceptedSocket) {
        perror("malloc failed");
        return NULL;
    }

    acceptedSocket->address = clientAddress;
    acceptedSocket->acceptedSocketFD = clientSocketFD;
    acceptedSocket->acceptedSuccessfully = (clientSocketFD > 0);

    if (!acceptedSocket->acceptedSuccessfully) {
        acceptedSocket->error = clientSocketFD;
    }

    return acceptedSocket;
}

int main() {
    int serverSocketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocketFD < 0) {
        perror("socket failed");
        return 1;
    }

    struct sockaddr_in serverAdd;
    serverAdd.sin_family = AF_INET;
    serverAdd.sin_addr.s_addr = INADDR_ANY;
    serverAdd.sin_port = htons(3000);

    int result = bind(serverSocketFD, (struct sockaddr *)&serverAdd, sizeof(serverAdd));
    if (result != 0) {
        perror("bind failed");
        close(serverSocketFD);
        return 1;
    }

    printf("Socket successfully bound\n");

    int listenResult = listen(serverSocketFD, 10);
    if (listenResult != 0) {
        perror("listen failed");
        close(serverSocketFD);
        return 1;
    }

    printf("Listening for connections...\n");
    startAcceptingIncomingConnections(serverSocketFD);

    shutdown(serverSocketFD, SHUT_RDWR);
    close(serverSocketFD);
    return 0;
}
