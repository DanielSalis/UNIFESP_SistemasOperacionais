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

typedef struct dirent dir_properties;

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
	int read_file = 1;
	int swap_file;
	int wrote_file;
	int file;
	unsigned char buffer[BLOCO];
	void *ptr_buff;
	while (read_file > 0)
	{
		read_file = read(file_origin, buffer, BLOCO);
		if (read_file == -1)
		{
			perror("read()");
			close(file_origin);
			close(file_destination);
			return 0;
		}
		else if (read_file > 0)
		{
			ptr_buff = buffer;
			wrote_file = read_file;
			swap_file = 0;
			for (wrote_file; wrote_file > 0; wrote_file -= file, swap_file += file)
			{
				file = write(file_destination, ptr_buff + swap_file, wrote_file);
				if (file == -1)
				{
					printf("\nERRO! Na escrita do arquivo\n");
					close(file_origin);
					close(file_destination);
					return 0;
				}
			};
		}
	};
	close(file_origin);
	close(file_destination);
}

int run_application(dir_properties **namelist, dir_properties **namelist_backup, int dir1, int dir2, char dir_origem[], char dir_dest[])
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
			char origem[255];
			strcpy(origem, dir_origem);
			j = 2;
			while (j < dir1)
			{
				char dest[255];
				strcpy(dest, dir_dest);
				if (strcmp(namelist[i]->d_name, namelist_backup[j]->d_name) == 0)
				{
					if (stat(strcat(origem, namelist[i]->d_name), &buf) != -1)
					{
						printf("%s\n", ctime(&buf.st_mtime));
					}
					if (stat(strcat(dest, namelist_backup[j]->d_name), &buf_backup) != -1)
					{
						printf("%s\n", ctime(&buf_backup.st_mtime));
					}

					start = buf.st_mtime;
					finish = buf_backup.st_mtime;

					if (compare_time(start, finish) > 0)
					{
						exec_sync(origem, dest);
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
	int dir1;
	int dir2;

	char dir_origem[255] = "minhapasta/";
	char dir_dest[255] = "minhapasta_backup/";

	dir_properties **fileList;
	dir_properties **fileList_backup;

	if (argc == 1)
	{
		dir1 = scandir(dir_origem, &fileList, NULL, alphasort);
		dir2 = scandir(dir_dest, &fileList_backup, NULL, alphasort);
	}
	if (dir1 == -1 || dir2 == -1)
		printf("\nFALHA! Diretório Inexistente\n");
	else
	{
		run_application(fileList, fileList_backup, dir1, dir2, dir_origem, dir_dest);
	}
	return 0;
}