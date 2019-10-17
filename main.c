#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#define MAX_SIZE 32
#define MAX_WORDS 3

#define verbose 1
/*
'0' : no info
'1' : info on child/parent process
'2' : adds info about splitting process
*/



/*
    TEST RUN :
                >>>gg
                (unrecognized command)

                >>>gedit
                (parent wait for child to execute)

                >>>gedit &
                (execute the child in background)

                >>>exit
                (exit the shell)
*/


//methods to change colors
void red()
{
    printf("\033[1;31m");
}

void blue()
{
    printf("\033[1;34m");
}

void yellow()
{
    printf("\033[1;33m");
}

void green()
{
    printf("\033[0;32m");
}

void reset()
{
    printf("\033[0m");
}

//change input string into exectuable code
void splitter(char str[], char **cmd)
{
    int flag = 0, index = 0;
    char delim[] = " ";



    // removes '\n' at the end of the string
    while (str[flag] != '\n')
        flag++;
    str[flag] = '\0';
    if (verbose >= 2)
    {
        yellow();
        printf("\n\nSPLITTING PROCESS:\n");
        printf("remove \\n str: %s \n", str);
        printf("\n\n");
    }
    char *ptr = strtok(str, delim);
    cmd[index++] = ptr;
    while (ptr != NULL)
    {
        yellow();
        if (verbose >= 2)
            printf("split[%d] = '%s'\n", index, ptr);
        ptr = strtok(NULL, delim);
        cmd[index++] = ptr;
    }

}

//find if the command works in the background or not
int check(char str[])
{
    for(int i = 0; i < strlen(str); i++)
    {
        if (str[i] == '&')
        {
            str[i] = '\0';
            return 1;
        }
    }
    return 0;
}

//log file for terminated process "children"
void foo(int signum)
{
    FILE *fp;
    fp = fopen("log.txt", "a");
    fprintf(fp, "Child process was terminated.\n");
    fclose(fp);
}

//clear the log file each new shell run
void newfile()
{
    FILE *fp;
    fp = fopen("log.txt", "w");
    fclose(fp);
}

int main()
{
    newfile();
    char str[MAX_SIZE];
    char *cmd[MAX_WORDS];
    int and_check = 0;
    pid_t child_pid;
    while (1)
    {
        printf("\n");
        green();
        printf(">>>");
        reset();
        fgets(str, MAX_SIZE, stdin);

        //check for empty input
        if(str[0] == '\n' || str[0] == ' ')
        {
            red();
            printf("unrecognized command !!!\n");
            reset();
            continue;
        }
        if (verbose >= 1)
        {
            yellow();
            printf("input String: %s", str);
        }
        and_check= check(str);
        splitter(str, cmd);
        reset();
        printf("\n");
        if (verbose >= 1)
        {
            int index = 0;
            while (*(cmd + index) != NULL)
            {
                yellow();
                printf("cmd[%d] = '%s'\n", index, cmd[index]);
                index++;
            }
            printf("[&] = %d\n", and_check);
        }

        if (strcmp(cmd[0], "exit") == 0)
        {
            blue();
            printf("Mission Failed, We'll Get 'Em Next Time\n\n");
            exit(0);
        }
        //start fork
        child_pid = fork();

        //child process
        if (child_pid == 0)
        {

            if (verbose >= 1  && and_check == 0)
            {
                yellow();
                printf("child --> pid = %d and ppid = %d\n", getpid(), getppid());
                reset();
            }
            //execute the command
            execvp(cmd[0], cmd);
            //returns if the call is successful
            red();
            printf("unrecognized command !!!\n");
            reset();
            exit(1);
        }

        //neither child nor parent
        else if (child_pid < 0)
        {
            perror("Fork failed");
            exit(1);
        }

        //parent
        else
        {
            if (verbose >= 1)
            {
                yellow();
                printf("parent --> pid = %d\n", getpid());
            }

            //send signal to create log file
            signal(SIGCHLD, foo);

            //wait in case process is working in foreground
            if(and_check == 0)
            {
                //because wait is not working on my OS
                //wait(NULL);
                waitpid(child_pid,NULL,0);
            }
        }
    }
    return 0;
}
