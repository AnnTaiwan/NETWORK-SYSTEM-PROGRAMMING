#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <errno.h>
#include <time.h>

#define FILENAME_LEN 16

// for sorting
int compare(const void *a, const void *b) {
    return (*(int *)a - *(int *)b);
}

double get_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <NF(Number of files)> <Directory>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int nf = atoi(argv[1]);
    char *dir = argv[2];
    int *file_indices = malloc(nf * sizeof(int));
    char filename[FILENAME_LEN];

    srand(time(NULL));

    // get random file name and creating order
    for (int i = 0; i < nf; i++) {
        file_indices[i] = rand() % 1000000;
    }

    // Create files randomly
    printf("Creating %d files in random order...\n", nf);
    double start_create = get_time();
    for (int i = 0; i < nf; i++) {
        snprintf(filename, FILENAME_LEN, "%s/x%06d", dir, file_indices[i]);
        int fd = open(filename, O_CREAT | O_WRONLY | O_TRUNC, 0644);
        if (fd == -1) {
            if (errno != EEXIST) { // Ignore duplication
                perror("open");
            }
        } else {
            write(fd, "a", 1); // wrote 1 byte
            close(fd);
        }
    }
    double end_create = get_time();
    double total_time = end_create - start_create;
    printf("Create time: %.4f seconds\n", end_create - start_create);

    // Sort the filename for deleting them in order
    qsort(file_indices, nf, sizeof(int), compare);

    // Delete files in increasing order
    printf("Deleting %d files in increasing order...\n", nf);
    double start_delete = get_time();
    for (int i = 0; i < nf; i++) {
        snprintf(filename, FILENAME_LEN, "%s/x%06d", dir, file_indices[i]);
        unlink(filename);
    }
    double end_delete = get_time();
    total_time += end_delete - start_delete;
    printf("Delete time: %.4f seconds\n", end_delete - start_delete);
    
    printf("Total time: %.4f seconds\n", total_time);
    free(file_indices);
    return 0;
}
