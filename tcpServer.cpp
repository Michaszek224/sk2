#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main() {
    struct sockaddr_in servaddr;

    // Create socket
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        perror("Socket failed");
        return 1;
    }

    // Allow address reuse
    int opt = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt failed");
        close(fd);
        return 1;
    }

    // Set up server address
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(4444);

    // Bind socket to the specified port
    int fail = bind(fd, (sockaddr*)&servaddr, sizeof(servaddr));
    if (fail == -1) {
        perror("Bind failed");
        close(fd);
        return 1;
    }

    // Listen for incoming connections
    fail = listen(fd, 1);
    if (fail) {
        perror("Listen failed");
        close(fd);
        return 1;
    }

    printf("Server listening on port 4444...\n");

    // Main loop to accept and handle client connections
    while (1) {
        char buffer[256];
        sockaddr_in clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);

        // Accept a connection
        int clientFd = accept(fd, (sockaddr*)&clientAddr, &clientAddrLen);
        if (clientFd == -1) {
            perror("Accept failed");
            continue; // Don't exit, continue to the next iteration
        }

        printf("Connection from %s:%hu\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

        // Receive data from the client
        int bytesReceived = recv(clientFd, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived > 0) {
            printf("Received: %s\n", buffer);

            // Send a response to the client
            const char* response = "Message received\n";
            if (send(clientFd, response, strlen(response), 0) == -1) {
                perror("Send failed");
            }
        } else if (bytesReceived == 0) {
            printf("Client disconnected.\n");
        } else {
            perror("Receive failed");
        }

        // Close the client connection
        shutdown(clientFd, SHUT_RDWR);
        close(clientFd);
    }

    // Close the server socket
    close(fd);
    return 0;
}
