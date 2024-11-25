/* Exercise 54.7:
   Rewrite the programs in Listing 48-2 (svshm_xfr_writer.c) and Listing 48-3
   (svshm_xfr_reader.c) to use POSIX shared memory objects instead of System V
   shared memory.
*/
/*  posix_shm.h

   Header file used by the posix_shm_reader.c and posix_shm_writer.c programs.
*/
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
/* Hard-coded keys for IPC objects */

#define SHM_NAME "/demo_posix_shm" /* Name for shared memory segment */

#define OBJ_PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)
                                /* Permissions for our IPC objects */

/* Two semaphores are used to ensure exclusive, alternating access
   to the shared memory segment */

#define SEM_WRITE "/sem_write"             /* Writer has access to shared memory */
#define SEM_READ "/sem_read"              /* Reader has access to shared memory */

#ifndef BUF_SIZE                /* Allow "cc -D" to override definition */
#define BUF_SIZE 1024           /* Size of transfer buffer */
#endif

struct shmseg {                 /* Defines structure of shared memory segment */
    int cnt;                    /* Number of bytes used in 'buf' */
    char buf[BUF_SIZE];         /* Data being transferred */
};
