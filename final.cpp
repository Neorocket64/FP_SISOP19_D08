#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <pthread.h>
#include <limits.h>

pthread_t tid[1001];

//function
char min[3], hor[3], day[3], mon[3], dyw[2];
char execp[1000];

void* launch_exec(void *args)
{
    char *execp = (char *) args;
    //system
    system(execp);
    
    return;
}

bool check_time()
{
	if
	(
		//note : ? if 1, then left side is executed, else do the right side
		(strcmp(min, "*") == 0 ? 1 : atoi(min) == tmstmp->tm_min) &&
		(strcmp(hor, "*") == 0 ? 1 : atoi(hor) == tmstmp->tm_hour) &&
		(strcmp(day, "*") == 0 ? 1 : atoi(day) == tmstmp->tm_mday) &&
		(strcmp(mon, "*") == 0 ? 1 : atoi(mon) == (tmstmp->tm_mon + 1)) &&
		(strcmp(dyw, "*") == 0 ? 1 : atoi(dyw) == tmstmp->tm_wday)
	)
	return TRUE;
	else FALSE;
}

int main()
{
    char curPath[PATH_MAX], tabfile[PATH_MAX];
    if(getcwd(curPath, sizeof(curPath)) == NULL)
	{
        perror("[error - getcwd]");
    }
    strcpy(tabfile, curPath);
    strcat(tabfile, "/crontab.data");
    
    pid_t pid, sid;
    pid = fork();
    if (pid < 0)
	{
        exit(EXIT_FAILURE);
    }
    if (pid > 0)
	{
        exit(EXIT_SUCCESS);
    }
    umask(0);
    sid = setsid();
    if (sid < 0)
	{
        exit(EXIT_FAILURE);
    }
    if ((chdir("/")) < 0)
	{
        exit(EXIT_FAILURE);
    }

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
	
	int lined = 0;
	
    while(1)
	{
        time_t timestmp;
        struct tm *tmstmp;
        //time local
        time(&timestmp);
        tmstmp = localtime(&rawtime);
		
		//check if file isn't empty
		FILE *config = fopen(tabfile, "r");
		if(config != NULL)
		{
			//check if the second is 0 (for exact timing)
            if (tmstmp->tm_sec == 0)
			{
                while(EOF != fscanf(config, "%2s %2s %2s %2s %2s %[^\r\n]", min, hor, day, mon, dyw, execp))
				{
                    if (fgetc(config) != '\n') getc(config);
                    //checking time if it's the time
                    if(check_time())
					{
                        int terr;
                        terr = pthread_create(&(tid[lined]), NULL, &launch_exec, &execp);
                        if(terr != 0)
						{
                            printf("can't create thread[%d] : [%s]\n",lined, strerror(terr));
                        }
						else
                        
                        terr = pthread_join(tid[lined], NULL);
                        if(terr != 0)
						{
                            printf("can't join thread[%d] : [%s]\n",lined, strerror(terr));
                        }
                        lined++;
                    }
                }
                fclose(config);
            }
        }
        else perror("error occured while opening file");

        sleep(1);
    }
    exit(EXIT_SUCCESS);
}
