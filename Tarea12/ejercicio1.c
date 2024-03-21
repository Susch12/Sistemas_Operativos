#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

struct DATA {
    int x, y;
    char car;
};

void proceso_L(int pipe_fd);
void proceso_E();

int main() {
    int pipe_fd[2];
    pid_t pid;

    // Crear tubería
    if (pipe(pipe_fd) == -1) {
        perror("Error al crear la tubería");
        return 1;
    }

    // Crear proceso L
    pid = fork();

    if (pid < 0) {
        fprintf(stderr, "Error al crear proceso L\n");
        return 1;
    } else if (pid == 0) {
        // Proceso hijo (E)
        close(pipe_fd[0]); // Cerrar extremo de lectura de la tubería en el proceso hijo
        proceso_E(pipe_fd[1]); // Pasar el extremo de escritura de la tubería al proceso hijo
        exit(EXIT_SUCCESS);
    } else {
        // Proceso padre (L)
        close(pipe_fd[1]); // Cerrar extremo de escritura de la tubería en el proceso padre
        proceso_L(pipe_fd[0]); // Pasar el extremo de lectura de la tubería al proceso padre
        wait(NULL);
    }

    return 0;
}

void proceso_L(int pipe_fd) {
    struct DATA data;
    int suma;

    // Leer estructuras y realizar la suma de x e y de cada una
    for (int i = 0; i < 10; i++) {
        // Se lee la estructura completa
        if (read(pipe_fd, &data, sizeof(struct DATA)) == -1) {
            perror("Error al leer la estructura");
            exit(EXIT_FAILURE);
        }
        suma = data.x + data.y;
        printf("Proceso L: Suma de x e y = %d, car = %c\n", suma, data.car);
    }

    close(pipe_fd); // Cerrar el extremo de lectura de la tubería
}

void proceso_E(int pipe_fd) {
    struct DATA data;

    // Escribir 10 estructuras
    for (int i = 0; i < 10; i++) {
        data.x = i;
        data.y = i * 2;
        data.car = 'A' + i;
        // Se escribe la estructura completa
        if (write(pipe_fd, &data, sizeof(struct DATA)) == -1) {
            perror("Error al escribir la estructura");
            exit(EXIT_FAILURE);
        }
    }

    close(pipe_fd); // Cerrar el extremo de escritura de la tubería
}
