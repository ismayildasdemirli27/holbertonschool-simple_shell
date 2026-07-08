#include "shell.h"

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
	struct stat buffer;

	if (stat(command, &buffer) == 0)
	{
		file_path = malloc(strlen(command) + 1);
		return (file_path ? strcpy(file_path, command) : NULL);
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
		file_path = malloc(strlen(path_token) + strlen(command) + 2);
		if (!file_path)
			break;
		sprintf(file_path, "%s/%s", path_token, command);
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
 * execute_cmd - Executes the command using fork and execve
 * @args: Arguments array
 * @argv0: Name of the shell program
 * @line: The raw input line (for freeing on exit)
 */
void execute_cmd(char **args, char *argv0, char *line)
{
	char *cmd_path;
	pid_t child_pid;
	int status;

	cmd_path = get_location(args[0]);
	if (!cmd_path)
	{
		perror(argv0);
		return;
	}
	child_pid = fork();
	if (child_pid == 0)
	{
		execve(cmd_path, args, environ);
		perror(argv0);
		free(cmd_path);
		free(line);
		exit(127);
	}
	else
		wait(&status);
	free(cmd_path);
}

/**
 * main - A basic UNIX command line interpreter
 * @argc: Number of arguments
 * @argv: Array of arguments
 *
 * Return: 0 on success
 */
int main(int argc, char **argv)
{
	char *line = NULL, *token, *args[128];
	size_t len = 0;
	int i;

	(void)argc;
	while (1)
	{
		if (isatty(STDIN_FILENO))
			printf("#cisfun$ ");
		if (getline(&line, &len, stdin) == -1)
		{
			if (isatty(STDIN_FILENO))
				printf("\n");
			break;
		}
		line[strcspn(line, "\n")] = '\0';
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
		if (strcmp(args[0], "exit") == 0)
			break;
		execute_cmd(args, argv[0], line);
	}
	free(line);
	return (0);
}
