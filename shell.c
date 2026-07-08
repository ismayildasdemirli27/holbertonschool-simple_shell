#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

extern char **environ;

/**
 * main - A basic UNIX command line interpreter with arguments
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
	char *args[128]; /* Maksimum 128 arqument qəbul edə bilən massiv */
	char *token;

	(void)argc; /* İstifadə olunmayan dəyişən xəbərdarlığını ləğv etmək üçün */

	while (1)
	{
		if (isatty(STDIN_FILENO))
			printf("#cisfun$ ");

		read = getline(&line, &len, stdin);
		
		/* EOF (Ctrl+D) yoxlaması */
		if (read == -1)
		{
			if (isatty(STDIN_FILENO))
				printf("\n");
			break;
		}

		/* getline-ın əlavə etdiyi yeni sətir simvolunu silmək */
		if (line[read - 1] == '\n')
			line[read - 1] = '\0';

		/* Sətri boşluqlara və tablara görə hissələrə ayırmaq (Tokenization) */
		i = 0;
		token = strtok(line, " \t");
		while (token != NULL)
		{
			args[i] = token;
			i++;
			token = strtok(NULL, " \t");
		}
		args[i] = NULL; /* Arqumentlər massivini NULL ilə bitirmək şərtdir */

		/* Əgər istifadəçi sadəcə "Enter" basıbsa və ya boşluq yazıbsa, dövrü davam et */
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
			/* Uşaq proses (Child process): komandanı icra et */
			if (execve(args[0], args, environ) == -1)
			{
				perror(argv[0]);
				free(line);
				exit(127); /* Komanda tapılmadı xətası */
			}
		}
		else
		{
			/* Valideyn proses (Parent process): uşağın işini bitirməsini gözlə */
			wait(&status);
		}
	}

	free(line);
	return (0);
}
