/* Exercise 54.7:
   Rewrite the programs in Listing 48-2 (svshm_xfr_writer.c) and Listing 48-3
   (svshm_xfr_reader.c) to use POSIX shared memory objects instead of System V
   shared memory.
*/
/* posix_shm_reader.c

   Read data from a POSIX shared memory using a binary semaphore lock-step
   protocol; see posix_shm_writer.c
*/
#include "posix_shm.h"
void errExit(const char*msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}
int main(int argc, char *argv[])
{
    int xfrs, bytes;
    struct shmseg *shmp;
    sem_t *semRead, *semWrite;
    int fd; // POSIX shared memory file descriptor

    /* Open the shared memory object */
    fd = shm_open(SHM_NAME, O_RDONLY, OBJ_PERMS);
    if (fd == -1)
        errExit("shm_open");

    /* Map the shared memory into address space */
    shmp = mmap(NULL, sizeof(struct shmseg), PROT_READ, MAP_SHARED, fd, 0);
    if (shmp == MAP_FAILED)
        errExit("mmap");

    close(fd); // Can close FD after mmap

    /* Open the semaphores */
    semWrite = sem_open(SEM_WRITE, 0);
    if (semWrite == SEM_FAILED)
        errExit("sem_open - write semaphore");

    semRead = sem_open(SEM_READ, 0);
    if (semRead == SEM_FAILED)
        errExit("sem_open - read semaphore");

    /* Transfer blocks of data from shared memory to stdout */
    for (xfrs = 0, bytes = 0; ; xfrs++) {
        if (sem_wait(semRead) == -1) // Wait until the writer gives the turn
            errExit("sem_wait - read");

        if (shmp->cnt == 0) // Writer signals EOF
            break;

        bytes += shmp->cnt;

        if (write(STDOUT_FILENO, shmp->buf, shmp->cnt) != shmp->cnt)
            errExit("partial/failed write");

        if (sem_post(semWrite) == -1) // Signal the writer to write more
            errExit("sem_post - write");
    }
    /* Give writer one more turn, so it can clean up */
    if (sem_post(semWrite) == -1) 
        errExit("sem_post - write");

    /* Cleanup */
    if (sem_close(semRead) == -1)
        errExit("sem_close - read semaphore");
    if (sem_close(semWrite) == -1)
        errExit("sem_close - write semaphore");
    if (munmap(shmp, sizeof(struct shmseg)) == -1)
        errExit("munmap");

    printf("Received %d bytes (%d transfers)\n", bytes, xfrs);
    exit(EXIT_SUCCESS);
}
