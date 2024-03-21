#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    int fd_parent_to_child[2]; // Pipe para comunicación del padre al hijo
    int fd_child_to_parent[2]; // Pipe para comunicación del hijo al padre

    // Crear el primer pipe (padre al hijo)
    if (pipe(fd_parent_to_child) == -1) {
        perror("Error al crear el primer pipe");
        exit(EXIT_FAILURE);
    }

    // Crear el segundo pipe (hijo al padre)
    if (pipe(fd_child_to_parent) == -1) {
        perror("Error al crear el segundo pipe");
        exit(EXIT_FAILURE);
    }

    pid_t pid; // Identificador del proceso

    // Crear proceso hijo (M)
    pid = fork();
    if (pid == -1) {
        perror("Error al hacer fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) { // Proceso hijo (M)
        close(fd_parent_to_child[1]); // Cerrar extremo de escritura del primer pipe (padre al hijo)
        close(fd_child_to_parent[0]); // Cerrar extremo de lectura del segundo pipe (hijo al padre)

        // Leer desde el pipe (mensaje del padre)
        char buffer_parent[100];
        read(fd_parent_to_child[0], buffer_parent, sizeof(buffer_parent));
        printf("Mensaje del padre: %s\n", buffer_parent);

        // Enviar mensaje al padre
        char message_child[] = "Hola desde el hijo";
        write(fd_child_to_parent[1], message_child, sizeof(message_child));

        close(fd_parent_to_child[0]); // Cerrar extremo de lectura del primer pipe
        close(fd_child_to_parent[1]); // Cerrar extremo de escritura del segundo pipe
        exit(EXIT_SUCCESS);
    } else { // Proceso padre (R)
        close(fd_parent_to_child[0]); // Cerrar extremo de lectura del primer pipe (padre al hijo)
        close(fd_child_to_parent[1]); // Cerrar extremo de escritura del segundo pipe (hijo al padre)

        printf("Mensaje desde el padre\n");

        // Enviar mensaje al hijo
        char message_parent[] = "Hola desde el padre";
        write(fd_parent_to_child[1], message_parent, sizeof(message_parent));

        // Leer desde el pipe (mensaje del hijo)
        char buffer_child[100];
        read(fd_child_to_parent[0], buffer_child, sizeof(buffer_child));
        printf("Mensaje desde el hijo: %s\n", buffer_child);

        // Esperar a que el proceso hijo termine
        wait(NULL);

        close(fd_parent_to_child[1]); // Cerrar extremo de escritura del primer pipe
        close(fd_child_to_parent[0]); // Cerrar extremo de lectura del segundo pipe
        exit(EXIT_SUCCESS);
    }

    return 0;
}
