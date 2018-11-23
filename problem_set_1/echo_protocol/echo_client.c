#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8888

int main(void)
{
    // Create a file descriptor for the socket.
    int client_fd;
    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Could not create socket");
        exit(EXIT_FAILURE);
    }

    // Name the socket.
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port   = htons(PORT);

    if (inet_pton(AF_INET, "0.0.0.0", &address.sin_addr) <= 0) {
        perror("Invalid address/Address not supported");
        exit(EXIT_FAILURE);
    }

    // Open a connection on the socket.
    if (connect(client_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Could not open a connection");
        close(client_fd);
        exit(EXIT_FAILURE);
    }

    printf("Connected successfully!\n");

    // Send a message to the server.
    char message[] = "hellO, woRlD!";
    write(client_fd, message, strlen(message));

    // Get the response back from the server.
    char response[1000];
    read(client_fd, response, 1000);
    printf("Response from server: %s\n", response);

    // Close the socket and exit successfully.
    close(client_fd);
    exit(EXIT_SUCCESS);
}
