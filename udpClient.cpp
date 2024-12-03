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

    // Set up server address
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(4444);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");  // Server IP address

    // Prepare message to send
    const char* message = "Hello, UDP Server!";
    printf("Sending message: %s\n", message);

    // Send data to the server
    if (sendto(fd, message, strlen(message), 0, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1) {
        perror("Send failed");
        close(fd);
        return 1;
    }

    // Buffer for receiving response
    char buffer[256];
    socklen_t serverAddrLen = sizeof(servaddr);

    // Receive response from the server
    int bytesReceived = recvfrom(fd, buffer, sizeof(buffer) - 1, 0, (struct sockaddr*)&servaddr, &serverAddrLen);
    if (bytesReceived > 0) {
        buffer[bytesReceived] = '\0'; // Null-terminate the received message
        printf("Received from server: %s\n", buffer);
    } else {
        perror("Receive failed");
    }

    // Close the socket
    close(fd);
    return 0;
}
