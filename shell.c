#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

extern char **environ;

/**
 * main - A basic UNIX command line interpreter
 * @argc: The number of arguments passed to the program
 * @argv: An array of pointers to the arguments
 *
 * Return: 0 on success, or an error code on failure
 */
int main(int argc, char **argv)
{
	char *line = NULL;
	size_t len = 0;
	ssize_t read;
	pid_t child_pid;
	int status, i;
	char *args[128];
	char *token;

	(void)argc;

	while (1)
	{
		if (isatty(STDIN_FILENO))
			printf("#cisfun$ ");

		read = getline(&line, &len, stdin);
		if (read == -1)
		{
			if (isatty(STDIN_FILENO))
				printf("\n");
			break;
		}

		if (line[read - 1] == '\n')
			line[read - 1] = '\0';

		i = 0;
		token = strtok(line, " \t");
		while (token != NULL)
		{
			args[i] = token;
			i++;
			token = strtok(NULL, " \t");
		}
		args[i] = NULL;

		if (args[0] == NULL)
			continue;

		child_pid = fork();
		if (child_pid == -1)
		{
			perror("Fork failed");
			continue;
		}

		if (child_pid == 0)
		{
			if (execve(args[0], args, environ) == -1)
			{
				perror(argv[0]);
				free(line);
				exit(127);
			}
		}
		else
		{
			wait(&status);
		}
	}

	free(line);
	return (0);
}
