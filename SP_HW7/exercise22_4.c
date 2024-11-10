#include "system_v.h"

void signal_handler(int sig) {
    printf("RECEIVED SIGNAL %d\n", sig);
}

int main() {
    // Test my_sigset() - Setting up a custom signal handler for SIGINT
    printf("Setting up custom signal handler for SIGINT\n");
    my_sigset(SIGINT, signal_handler); // set a custom handler for SIGINT
    
    // Test my_sighold() - Blocking SIGINT signal
    printf("Blocking SIGINT signal\n");
    my_sighold(SIGINT); // block SIGINT, system can receive SIGINT, but it won't invoke handler

    // Try to send SIGINT signal (Ctrl+C), but it will be blocked
    printf("Press Ctrl+C to send SIGINT, but it will be blocked...\n");
    sleep(5); // Wait for a few seconds to simulate blocked signal

    // Test my_sigrelse() - Unblocking SIGINT signal
    printf("Unblocking SIGINT signal\n");
    my_sigrelse(SIGINT); // unblock SIGINT. After unblock the signal, the previous SIGINT will invoke the handler
    // so here will print the handler info
  
    // Now SIGINT should be received and handled by signal_handler
    printf("Press Ctrl+C to send SIGINT again, now it should be received...\n");
    sleep(5); // Wait for SIGINT

    // Test my_sigignore() - Ignore SIGTERM signal
    printf("Setting up to ignore SIGTERM signal\n");
    my_sigignore(SIGTERM); // Ignore SIGTERM

    // Try to send SIGTERM signal, but it should be ignored
    printf("Sending SIGTERM signal by `kill` this process, it should be ignored...\n");
    kill(getpid(), SIGTERM);
    sleep(1); // Give it a moment to see no handler is invoked

    // Test my_sigpause() - Pause until SIGUSR1 signal is received
    printf("Waiting for SIGUSR1 signal using my_sigpause\n");
    kill(getpid(), SIGUSR1);  // Send SIGUSR1 before pausing
    my_sigpause(SIGUSR1); // wait for SIGUSR1 to be received
    printf("SIGUSR1 received, continuing...\n");

    return 0;
}

