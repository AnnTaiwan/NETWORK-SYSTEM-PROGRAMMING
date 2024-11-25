/* Exercise 54.7:
   Rewrite the programs in Listing 48-2 (svshm_xfr_writer.c) and Listing 48-3
   (svshm_xfr_reader.c) to use POSIX shared memory objects instead of System V
   shared memory.
*/
/*  posix_shm_writer.c

   Read buffers of data data from standard input into a POSIX shared memory
   segment from which it is copied by posix_shm_reader.c

   We use a pair of binary semaphores to ensure that the writer and reader have
   exclusive, alternating access to the shared memory. (I.e., the writer writes
   a block of text, then the reader reads, then the writer writes etc). This
   ensures that each block of data is processed in turn by the writer and
   reader.

   This program needs to be started before the reader process as it creates the
   shared memory and semaphores used by both processes.

   Together, these two programs can be used to transfer a stream of data through
   shared memory as follows:

        $ posix_shm_writer < infile &
        $ posix_shm_reader > out_file
*/
#include "posix_shm.h"
void errExit(const char*msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}
int main(int argc, char *argv[])
{
    int fd;                      // POSIX shared memory file descriptor
    struct shmseg *shmp;         // Pointer to shared memory segment
    sem_t *semRead, *semWrite;   // POSIX semaphores
    int bytes, xfrs;             // Track bytes and transfers
        
    /* Create the shared memory object */
    fd = shm_open(SHM_NAME, O_CREAT | O_EXCL | O_RDWR, OBJ_PERMS);
    if (fd == -1) {
        if (errno == EEXIST) { // Shared memory already exists, unlink and recreate
            printf("Shared memory already exists. Deleting it...\n");
            if (shm_unlink(SHM_NAME) == -1)
                errExit("shm_unlink");

            fd = shm_open(SHM_NAME, O_CREAT | O_EXCL | O_RDWR, OBJ_PERMS);
            if (fd == -1)
                errExit("shm_open");
        } else {
            errExit("shm_open");
        }
    }

    printf("Shared memory created successfully.\n");

    /* Set the size of the shared memory object */
    if (ftruncate(fd, sizeof(struct shmseg)) == -1)
        errExit("ftruncate");

    /* Map the shared memory object */
    shmp = mmap(NULL, sizeof(struct shmseg), PROT_WRITE, MAP_SHARED, fd, 0);
    if (shmp == MAP_FAILED)
        errExit("mmap");

    close(fd); // File descriptor is no longer needed after mmap

    /* Create and initialize semaphores */
    semWrite = sem_open(SEM_WRITE, O_CREAT, OBJ_PERMS, 1); // Initially writable
    if (semWrite == SEM_FAILED)
        errExit("sem_open - write semaphore");

    semRead = sem_open(SEM_READ, O_CREAT, OBJ_PERMS, 0); // Initially unreadable
    if (semRead == SEM_FAILED)
        errExit("sem_open - read semaphore");

    /* Transfer blocks of data from stdin to shared memory */
    for (xfrs = 0, bytes = 0; ; xfrs++) {
        if (sem_wait(semWrite) == -1) // Wait for the writer's turn
            errExit("sem_wait - write");

        shmp->cnt = read(STDIN_FILENO, shmp->buf, BUF_SIZE);
        if (shmp->cnt == -1)
            errExit("read");
        bytes += shmp->cnt;

        if (sem_post(semRead) == -1) // Signal the reader
            errExit("sem_post - read");

        /* EOF detection */
        if (shmp->cnt == 0) // End of file
            break;
    }
    /* Wait for the reader to finish */
    if (sem_wait(semWrite) == -1)
        errExit("sem_wait - final write");

    /* Clean up resources */
    if (sem_close(semRead) == -1)
        errExit("sem_close - read semaphore");
    if (sem_close(semWrite) == -1)
        errExit("sem_close - write semaphore");
    if (sem_unlink(SEM_READ) == -1)
        errExit("sem_unlink - read semaphore");
    if (sem_unlink(SEM_WRITE) == -1)
        errExit("sem_unlink - write semaphore");
    if (munmap(shmp, sizeof(struct shmseg)) == -1)
        errExit("munmap");
    if (shm_unlink(SHM_NAME) == -1)
        errExit("shm_unlink");

    printf("Sent %d bytes (%d transfers)\n", bytes, xfrs);
    exit(EXIT_SUCCESS);
}
