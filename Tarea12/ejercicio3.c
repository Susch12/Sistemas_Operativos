#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define NUM_CHILDREN 7

struct DATA {
    int x;
};

int main() {
    int pipe_fd[2];
    pid_t pid[NUM_CHILDREN];

    // Crear la tubería
    if (pipe(pipe_fd) == -1) {
        perror("Error creando la tubería");
        exit(EXIT_FAILURE);
    }

    // Crear procesos
    for (int i = 0; i < NUM_CHILDREN; i++) {
        pid[i] = fork();

        if (pid[i] == -1) {
            perror("Error creando proceso hijo");
            exit(EXIT_FAILURE);
        }

        if (pid[i] == 0) {
            // Proceso hijo
            if (i == 0) {
                // Proceso K
                sleep(8); // Temporizar por 8 segundos
                exit(EXIT_SUCCESS);
            } else if (i == 1 || i == 2) {
                // Procesos R y S
                close(pipe_fd[0]); // Cerrar extremo de lectura
                struct DATA data;
                for (int j = 1; ; j++) {
                    data.x = j;
                    write(pipe_fd[1], &data, sizeof(data));
                    sleep(1); // Simular escritura infinita
                }
            } else {
                // Procesos T, U, V, W, X
                close(pipe_fd[1]); // Cerrar extremo de escritura
                struct DATA received_data;
                while (1) {
                    read(pipe_fd[0], &received_data, sizeof(received_data));
                    printf("Soy proceso %c, leyendo dato: %d\n", 'U' + (i - 3), received_data.x);
                }
            }
        }
    }

    // Proceso padre
    close(pipe_fd[0]); // Cerrar extremo de lectura
    close(pipe_fd[1]); // Cerrar extremo de escritura
    // Esperar a que todos los procesos hijos terminen
    for (int i = 0; i < NUM_CHILDREN; i++) {
        waitpid(pid[i], NULL, 0);
    }

    return 0;
}
