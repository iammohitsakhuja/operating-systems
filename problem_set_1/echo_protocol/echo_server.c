#include <ctype.h>
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
    int server_fd, new_socket;
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Could not create socket");
        exit(EXIT_FAILURE);
    }

    // Name the socket.
    struct sockaddr_in address;
    int                address_len = sizeof(address);

    memset(&address, 0, sizeof(address));
    address.sin_family      = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port        = htons(PORT);

    // Bind the socket to the address.
    if (bind(server_fd, (struct sockaddr *)&address, address_len) < 0) {
        perror("Binding failed");
        exit(EXIT_FAILURE);
    }

    // Listen to connections from the particular address.
    if (listen(server_fd, 1) < 0) {
        perror("Listening failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Listening to connections on port: %i\n", PORT);

    // Accept all incoming connections on the socket.
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&address_len)) < 0) {
        perror("Accepting failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Client connected\n");

    // Get the message from the client.
    char message[1000] = { 0 };
    if (read(new_socket, message, 1000) < 0) {
        perror("No bytes to read");
        exit(EXIT_SUCCESS);
    }
    printf("Message from client: %s\n", message);

    // Convert the message to uppercase.
    for (char *ptr = message; *ptr != '\0'; ptr++)
        *ptr = toupper(*ptr);

    // Send the response back to the client.
    write(new_socket, message, strlen(message));

    // Close the sockets and exit successfully.
    close(new_socket);
    close(server_fd);
    exit(EXIT_SUCCESS);
}
