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
	size_t len;

	if (!environ || !name)
		return (NULL);

	len = strlen(name);
	while (environ[i])
	{
		if (strncmp(environ[i], name, len) == 0 && environ[i][len] == '=')
			return (environ[i] + len + 1);
		i++;
	}
	return (NULL);
}

/**
 * print_error - Prints standard shell error for command not found
 * @argv0: Name of the shell program
 * @cmd: The command that was not found
 */
void print_error(char *argv0, char *cmd)
{
	write(STDERR_FILENO, argv0, strlen(argv0));
	write(STDERR_FILENO, ": 1: ", 5);
	write(STDERR_FILENO, cmd, strlen(cmd));
	write(STDERR_FILENO, ": not found\n", 12);
}

/**
 * get_location - Searches for the command in the PATH directories
 * @command: The command to locate
 *
 * Return: Dynamically allocated string containing full path, or NULL
 */
char *get_location(char *command)
{
	char *path, *path_copy, *t, *file_path;
	struct stat buffer;

	if (!command)
		return (NULL);
	if (strchr(command, '/'))
	{
		if (stat(command, &buffer) == 0)
		{
			file_path = malloc(strlen(command) + 1);
			return (file_path ? strcpy(file_path, command) : NULL);
		}
		return (NULL);
	}
	path = _getenv("PATH");
	if (!path || !*path)
		return (NULL);
	path_copy = malloc(strlen(path) + 1);
	if (!path_copy)
		return (NULL);
	strcpy(path_copy, path);
	t = strtok(path_copy, ":");
	while (t)
	{
		file_path = malloc(strlen(t) + strlen(command) + 2);
		sprintf(file_path, "%s/%s", t, command);
		if (stat(file_path, &buffer) == 0)
		{
			free(path_copy);
			return (file_path);
		}
		free(file_path);
		t = strtok(NULL, ":");
	}
	free(path_copy);
	return (NULL);
}

/**
 * execute_cmd - Executes the command using fork and execve
 * @args: Arguments array
 * @argv0: Name of the shell program
 * @line: The raw input line (for freeing on exit)
 *
 * Return: The exit status of the executed command
 */
int execute_cmd(char **args, char *argv0, char *line)
{
	char *cmd_path;
	pid_t child_pid;
	int status = 0;
	char *envp[] = {NULL};

	cmd_path = get_location(args[0]);
	if (!cmd_path)
	{
		print_error(argv0, args[0]);
		return (127);
	}
	child_pid = fork();
	if (child_pid == 0)
	{
		execve(cmd_path, args, environ ? environ : envp);
		perror(argv0);
		free(cmd_path);
		free(line);
		exit(127);
	}
	else
	{
		wait(&status);
		if (WIFEXITED(status))
			status = WEXITSTATUS(status);
	}
	free(cmd_path);
	return (status);
}

/**
 * main - A basic UNIX command line interpreter
 * @argc: Number of arguments
 * @argv: Array of arguments
 *
 * Return: 0 on success, or the exit status of the last command
 */
int main(int argc, char **argv)
{
	char *line = NULL, *token, *args[128];
	size_t len = 0;
	int i, last_status = 0;

	(void)argc;
	while (1)
	{
		if (isatty(STDIN_FILENO))
			write(STDOUT_FILENO, "#cisfun$ ", 9);
		if (getline(&line, &len, stdin) == -1)
		{
			if (isatty(STDIN_FILENO))
				write(STDOUT_FILENO, "\n", 1);
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
		{
			free(line);
			exit(last_status);
		}
		last_status = execute_cmd(args, argv[0], line);
	}
	free(line);
	return (last_status);
}
