#include "shell.h"

/**
 * main - UNIX command line interpreter
 * @ac: Argument count
 * @av: Argument vector
 *
 * Return: 0 on success
 */
int main(int ac __attribute__((unused)), char **av)
{
	char *line = NULL, *argv[2];
	size_t len = 0;
	ssize_t nread;
	pid_t child_pid;
	int status;

	while (1)
	{
		if (isatty(STDIN_FILENO))
			printf("#cisfun$ ");
		nread = getline(&line, &len, stdin);
		if (nread == -1)
		{
			if (isatty(STDIN_FILENO))
				printf("\n");
			break;
		}
		if (line[nread - 1] == '\n')
			line[nread - 1] = '\0';
		if (line[0] == '\0')
			continue;
		argv[0] = line;
		argv[1] = NULL;
		child_pid = fork();
		if (child_pid == 0)
		{
			if (execve(argv[0], argv, environ) == -1)
			{
				perror(av[0]);
				free(line);
				exit(2);
			}
		}
		else
			wait(&status);
	}
	free(line);
	return (0);
}
