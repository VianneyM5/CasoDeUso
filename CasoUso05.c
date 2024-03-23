#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#define PORT 8080
#define MAX_CLIENTS 5

void handle_client(int client_socket) {
    char buffer[1024] = {0};
    int valread;
    valread = read(client_socket, buffer, 1024);
    printf("Mensaje recibido del cliente: %s\n", buffer);
    send(client_socket, "¡Hola desde el servidor!", strlen("¡Hola desde el servidor!"), 0);
}

int main() {
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Crear socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Configurar socket
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

    // Enlazar socket
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Escuchar
    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // Manejo de clientes
    while(1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        pid_t pid = fork();
        if (pid == 0) { // Proceso hijo
            close(server_fd); // Cerramos el socket del servidor en el proceso hijo
            handle_client(new_socket); // Manejamos la comunicación con el cliente
            exit(0);
        } else if (pid < 0) { // Error en fork()
            perror("fork");
        } else { // Proceso padre
            close(new_socket); // Cerramos el socket del cliente en el proceso padre
        }
    }

    return 0;
