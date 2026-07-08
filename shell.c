#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/stat.h>

extern char **environ;

/**
 * _getenv - Gets the value of an environment variable
 * @name: The name of the variable to find
 *
 * Return: A pointer to the value, or NULL if not found
 */
char *_getenv(const char *name)
{
	int i = 0;
	size_t len = strlen(name);

	while (environ[i])
	{
		if (strncmp(environ[i], name, len) == 0 && environ[i][len] == '=')
			return (environ[i] + len + 1);
		i++;
	}
	return (NULL);
}

/**
 * get_location - Searches for the command in the PATH directories
 * @command: The command to locate
 *
 * Return: Dynamically allocated string containing full path, or NULL
 */
char *get_location(char *command)
{
	char *path, *path_copy, *path_token, *file_path;
	int cmd_len, dir_len;
	struct stat buffer;

	if (stat(command, &buffer) == 0)
	{
		file_path = malloc(strlen(command) + 1);
		if (file_path)
			strcpy(file_path, command);
		return (file_path);
	}
	path = _getenv("PATH");
	if (!path)
		return (NULL);
	path_copy = malloc(strlen(path) + 1);
	if (!path_copy)
		return (NULL);
	strcpy(path_copy, path);
	path_token = strtok(path_copy, ":");
	while (path_token != NULL)
	{
		dir_len = strlen(path_token);
		cmd_len = strlen(command);
		file_path = malloc(dir_len + cmd_len + 2);
		if (!file_path)
		{
			free(path_copy);
			return (NULL);
		}
		strcpy(file_path, path_token);
		strcat(file_path, "/");
		strcat(file_path, command);
		if (stat(file_path, &buffer) == 0)
		{
			free(path_copy);
			return (file_path);
		}
		free(file_path);
		path_token = strtok(NULL, ":");
	}
	free(path_copy);
	return (NULL);
}

/**
 * main - A basic UNIX command line interpreter
 * @argc: The number of arguments
 * @argv: An array of pointers to the arguments
 *
 * Return: 0 on success
 */
int main(int argc, char **argv)
{
	char *line = NULL, *token, *args[128], *cmd_path;
	size_t len = 0;
	ssize_t read;
	int status, i;
	pid_t child_pid;

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
		while (token)
		{
			args[i++] = token;
			token = strtok(NULL, " \t");
		}
		args[i] = NULL;
		if (!args[0])
			continue;
		cmd_path = get_location(args[0]);
		if (!cmd_path)
		{
			perror(argv[0]);
			continue;
		}
		child_pid = fork();
		if (child_pid == 0)
		{
			execve(cmd_path, args, environ);
			perror(argv[0]);
			free(cmd_path);
			free(line);
			exit(127);
		}
		else
			wait(&status);
		free(cmd_path);
	}
	free(line);
	return (0);
}
