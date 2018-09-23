#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <readline/readline.h>
#include <readline/history.h>

#define MAXCOM 1000
#define MAXLIST 100
void init()
{
	printf("\033[H\033[J");
	printf("\n\n\n\n******************"
        "************************"); 
	printf("\n\n\n Welcome To Utkarsh Ahuja Shell");
	printf("\n\n\n\n******************"
        "************************"); 
	char *username = getenv("USER");
	printf("User is@%s \n",username );
	sleep(1);
	printf("\033[H\033[J" );
}
void printDir()
{
	char dir[1024];
	getcwd(dir,sizeof(dir));
	printf("\nDir is: %s",dir);
}
void execute_no_pipe(char **parsedArgs)
{
	pid_t pid = fork();
	if(pid==-1)
	{
		printf("Fork failed\n");
		return;
	}
	else if(pid==0)
	{
		if(execvp(parsedArgs[0],parsedArgs)<0)
		{
			printf("Command cannot be executed\n");
		}
		exit(0);
	}
	else
	{
		wait(NULL);
		return;
	}
}

void execute_pipe(char **parsedArgs,char **parsedpipeArgs)
{
	int pipefd[2];
	pid_t p1,p2;
	if(pipe(pipefd) < 0)
	{
		printf("Pipe could not be initialized\n");
		return ;

	}
	p1 = fork();
	if(p1 < 0)
	{
		printf("Couldn't fork\n");
		return ;
	}
	if(p1==0)
	{
		close(pipefd[0]);
		dup2(pipefd[0], STDOUT_FILENO);
		close(pipefd[1]);
		if(execvp(parsedArgs[0],parsedArgs) < 0)
		{
			printf("Could not execute Command 1\n");
			exit(0);
		}
	}
	else
	{
		p2 = fork();
		if(p2 < 0)
		{
			printf("Couldn't fork\n");
			return;
		}
		if(p2==0)
		{
			close(pipefd[0]);
			dup2(pipefd[0],STDOUT_FILENO);
			close(pipefd[1]);
			if(execvp(parsedpipeArgs[0],parsedpipeArgs) < 0)
			{
				printf("Could not execute Command 1\n");
				exit(0);	
			}
		}
		else
		{
			wait(NULL);
			wait(NULL);
		}
	}
}
int takeinput(char *str)
{
	char *buffer = readline(">> ");
	if(strlen(buffer)!=0)
	{
		add_history(buffer);
		strcpy(str,buffer);
		return 0;
	}
	else
		return 1;
}
void open_help()
{ 
	puts("\n***WELCOME TO MY SHELL HELP***"
        "\nList of Commands supported:"
        "\n>cd"
        "\n>ls"
        "\n>exit"
        "\n>all other general commands available in UNIX shell"
        "\n>pipe handling"
        "\n>improper space handling"); 
}
int my_command_handler(char **parsed)
{
	int number_of_own_commands = 4,i,selected_command;
	char *list_of_commands[number_of_own_commands];
	char *username=getenv("USER");
	list_of_commands[0]="cd";
	list_of_commands[1]="hello";
	list_of_commands[2]="help";
	list_of_commands[3]="exit"; 
	for(i = 0;i < 4; ++i)
	{
		if(strcmp(parsed[0],list_of_commands[i])==0)
		{
			selected_command = i + 1;
			break;
		}
	}
	switch(selected_command)
	{
		case 1: chdir(parsed[1]);
				return 1;
		case 2: 
				printf("%sNever give up\n", username);
				return 1;
		case 3: open_help();
				return 1;
		case 4: printf("GoodBye.See you soon\n");
				exit(0);
		default: break;
	}
	return 0;
}
int command_pipe(char *inputstring,char **strpipe)
{
	int i;
	for(i = 0; i < 2; ++i)
	{
		strpipe[i] = strsep(&inputstring,"|");
		if(strpipe[i]==NULL)
			break;
	}
	if(strpipe[1]==NULL)
		return 0;
	else
		return 1;
}

void command_space(char *inputstring,char **str)
{
	int i;
	for(i = 0; i < MAXLIST; ++i)
	{
		str[i]=strsep(&inputstring," ");
		if(str[i]==NULL)
			break;
		if(strlen(str[i])==0)
			i--;
	}
}
int process_input_string(char *inputstring,char **parsed,char **parsedpipe)
{
	char *strpipe[2];
	int is_pipe=command_pipe(inputstring,strpipe );
	if(is_pipe)
	{
		command_space(strpipe[0],parsed);
		command_space(strpipe[1],parsedpipe);
	}
	else
	{
		command_space(inputstring,parsed);
	}
	if(my_command_handler(parsed))
		return 0;
	else
		return 1+is_pipe;
}

int main(int argc, char const *argv[])
{
	char inputstring[MAXCOM],*parsedArgs[MAXLIST],*parsedpipeArgs[MAXLIST];
	int execflag = 0;
	init();
	while(1)
	{
		printDir();
		if(takeinput(inputstring))
			continue;
		execflag = process_input_string(inputstring,parsedArgs,parsedpipeArgs);
		if(execflag==1)
			execute_no_pipe(parsedArgs);
		if(execflag==2)
			execute_pipe(parsedArgs,parsedpipeArgs);
	}
	return 0;
}