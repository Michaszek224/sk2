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
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
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

    printf("Server listening on port 4444...\n");

    // Main loop to handle client messages
    while (1) {
        char buffer[256];
        sockaddr_in clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);

        // Receive data from the client
        int bytesReceived = recvfrom(fd, buffer, sizeof(buffer) - 1, 0, (sockaddr*)&clientAddr, &clientAddrLen);
        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0'; // Null-terminate the received data
            printf("Received from %s:%hu - %s\n",
                   inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port), buffer);

            // Send a response to the client
            const char* response = "Message received\n";
            if (sendto(fd, response, strlen(response), 0, (sockaddr*)&clientAddr, clientAddrLen) == -1) {
                perror("Send failed");
            }
        } else {
            perror("Receive failed");
        }
    }

    // Close the server socket
    close(fd);
    return 0;
}
