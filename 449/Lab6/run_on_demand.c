#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>  // for printf
#include <unistd.h> // for pid_t
#include <signal.h> //

void sig_handler(int signum)
{

    char *argument;

    pid_t pid = fork();
    if (pid == 0)
    {
        if (signum == SIGUSR1)
        {
            argument = "ls";
            printf("Received signal %d.\n", signum);
            printf("Running %s.\n", argument);
            execlp("ls", "ls", (char *)NULL);
        }
        else if (signum == SIGUSR2)
        {
            argument = "ls -l -a";
            printf("Received signal %d.\n", signum);
            printf("Running %s.\n", argument);
            execlp("ls", "ls", "-l", "-a", (char *)NULL);
        }
    }

    else if (pid > 0)
    {
        wait(NULL);
        printf("Done!\n");
    }
    else
    {
        printf("error forking ");
        exit(EXIT_FAILURE);
    }
}

void exit_handler(int signum)
{
    printf("Leaving gracefully!\n");
    exit(EXIT_SUCCESS);
}

int main()
{

    signal(SIGUSR1, sig_handler);
    signal(SIGUSR2, sig_handler);
    signal(SIGINT, exit_handler);

    while (1)
    {
        pause();
    }

    return 0;
}
