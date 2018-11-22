#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8888

int main(void)
{
    // Create a socket.
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        fprintf(stderr, "Error occurred in creating the socket!\n");
        return 1;
    }

    // Create a server address.
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port   = htons(PORT);

    // Convert IP address from a presentation format to a network format.
    if (inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr) <= 0) {
        fprintf(stderr, "Invalid address/Address not supported!\n");
        close(sock);
        return 2;
    }

    // Open a connection to the given address on the socket.
    if (connect(sock, (const struct sockaddr *)&server_address, sizeof(server_address))) {
        fprintf(stderr, "Could not connect!\n");
        close(sock);
        return 3;
    }

    printf("Connected successfully!\n");
    close(sock);

    return 0;
}
