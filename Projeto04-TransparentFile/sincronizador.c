#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <fcntl.h>

#define BLOCO 4096

int compare_time(time_t start, time_t end)
{
	return start > end ? 1 : 0;
}

int exec_sync(char nome1[], char nome2[])
{
	printf("\n- - - - Sincronizando - - - -\n");
	int file_origin;
	int file_destination;
	file_origin = open(nome1, O_RDONLY);
	if (file_origin == -1)
	{
		printf("\nFALHA! Arquivo Inexistente\n");
		return 0;
	}
	file_destination = open(nome2, O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if (file_destination == -1)
	{
		printf("\nFALHA! Arquivo Inexistente\n");
		close(file_origin);
		return 0;
	}
	int nr;
	int ns;
	int nw;
	int n;
	unsigned char buffer[BLOCO];
	void *ptr_buff;
	do
	{
		nr = read(file_origin, buffer, BLOCO);
		if (nr == -1)
		{
			perror("read()");
			close(file_origin);
			close(file_destination);
			return 0;
		}
		else if (nr > 0)
		{
			ptr_buff = buffer;
			nw = nr;
			ns = 0;
			do
			{
				n = write(file_destination, ptr_buff + ns, nw);
				if (n == -1)
				{
					perror("write()");
					close(file_origin);
					close(file_destination);
					return 0;
				}
				ns += n;
				nw -= n;
			} while (nw > 0);
		}
	} while (nr > 0);
	close(file_origin);
	close(file_destination);
}

int run_application(struct dirent **namelist, struct dirent **namelist_backup, int dir1, int dir2, char dir_origem[], char dir_dest[])
{
	struct stat buf;
	struct stat buf_backup;
	int exec = 1;
	int i = 0;
	int j = 0;
	time_t start, finish;


	while (exec)
	{
		i = 2;
		while (i < dir1)
		{
			char nome1[255] = "minhapasta/";
			j = 2;
			while (j < dir1)
			{
				char nome2[255] = "minhapasta_backup/";
				if (strcmp(namelist[i]->d_name, namelist_backup[j]->d_name) == 0)
				{
					if (stat(strcat(nome1, namelist[i]->d_name), &buf) != -1)
					{
						printf("%s\n", ctime(&buf.st_mtime));
					}
					if (stat(strcat(nome2, namelist_backup[j]->d_name), &buf_backup) != -1)
					{
						printf("%s\n", ctime(&buf_backup.st_mtime));
					}

					start = buf.st_mtime;
					finish = buf_backup.st_mtime;

					if (compare_time(start, finish) > 0)
					{
						exec_sync(nome1, nome2);
					}
				}
				j++;
			}
			i++;
		}
		sleep(5);
	}
}

int main(int argc, char **argv)
{
	struct dirent **namelist;
	struct dirent **namelist_backup;

	int dir1;
	int dir2;

	char dir_origem[255] = "minhapasta/";
	char dir_dest[255] = "minhapasta_backup/";

	if (argc == 1)
	{
		dir1 = scandir(dir_origem, &namelist, NULL, alphasort);
		dir2 = scandir(dir_dest, &namelist_backup, NULL, alphasort);
	}
	if (dir1 < 0 || dir2 < 0)
		printf("\nFALHA! Diretório Inexistente\n");
	else
	{
		run_application(namelist, namelist_backup, dir1, dir2, dir_origem, dir_dest);
	}
	return 0;
}