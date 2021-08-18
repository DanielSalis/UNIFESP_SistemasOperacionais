#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <fcntl.h>

#define BUFFER_SIZE 3000

typedef struct dirent dir_properties;

int compare_time(time_t start, time_t end)
{
	return start > end ? 1 : 0;
}

int run_application(dir_properties **namelist, dir_properties **namelist_backup, int number_of_files_dir, int number_of_files_dir_backup, char dir_origem[], char dir_dest[], int force_exec);

int exec_sync(char original_file[], char backup_file[])
{
	printf("\n- - - - Sincronizando - - - -\n");
	int file_origin;
	int file_destination;
	file_origin = open(original_file, O_RDONLY);
	if (file_origin == -1)
	{
		printf("\nFALHA! Arquivo Inexistente\n");
		return 0;
	}
	file_destination = open(backup_file, O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
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
	unsigned char buffer[BUFFER_SIZE];
	void *ptr_buff;
	while (read_file > 0)
	{
		read_file = read(file_origin, buffer, BUFFER_SIZE);
		if (read_file == -1)
		{
			printf("\nERRO! Na leitura do arquivo\n");
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

int exec_folder_sync(dir_properties **namelist, dir_properties **namelist_backup, int number_of_files_dir, int number_of_files_dir_backup, char dir_origem[], char dir_dest[])
{
	int i = 2;
	int exec = 1;

	while (i < number_of_files_dir)
	{

		if (number_of_files_dir_backup == 2 || number_of_files_dir_backup < number_of_files_dir || strcmp(namelist[i]->d_name, namelist_backup[i]->d_name) != 0)
		{
			char concated_string[] = "minhapasta_backup/";
			char source[255];
			strcpy(source, namelist[i]->d_name);
			strcat(concated_string, source);
			FILE *out = fopen(concated_string, "w");
			fclose(out);
			number_of_files_dir_backup = scandir(dir_dest, &namelist_backup, NULL, alphasort);
			if (number_of_files_dir == number_of_files_dir_backup)
			{
				run_application(namelist, namelist_backup, number_of_files_dir, number_of_files_dir_backup, dir_origem, dir_dest, 1);
			}
		}
		i++;
	}
}

int run_application(dir_properties **dir_name_list, dir_properties **dir_name_list_backup, int number_of_files_dir, int number_of_files_dir_backup, char dir_origem[], char dir_dest[], int force_exec)
{
	struct stat buf;
	struct stat buf_backup;
	int exec = 1;
	int i = 0;
	int j = 0;
	int fs = force_exec;
	time_t start, finish;

	while (exec)
	{
		i = 2;
		while (i < number_of_files_dir)
		{
			char origem[255];
			strcpy(origem, dir_origem);
			j = 2;
			while (j < number_of_files_dir)
			{
				char dest[255];
				strcpy(dest, dir_dest);
				if (strcmp(dir_name_list[i]->d_name, dir_name_list_backup[j]->d_name) == 0)
				{
					if (stat(strcat(origem, dir_name_list[i]->d_name), &buf) != -1)
					{
						printf("Ultima atualização pasta original: %s\n", ctime(&buf.st_mtime));
					}
					if (stat(strcat(dest, dir_name_list_backup[j]->d_name), &buf_backup) != -1)
					{
						printf("Ultimo Backup: %s\n", ctime(&buf_backup.st_mtime));
					}

					start = buf.st_mtime;
					finish = buf_backup.st_mtime;

					int nmbr_files_d1 = scandir(dir_origem, &dir_name_list, NULL, alphasort);
					int nmbr_files_backup = scandir(dir_dest, &dir_name_list_backup, NULL, alphasort);

					if (nmbr_files_d1 != nmbr_files_backup)
					{
						exec_folder_sync(dir_name_list, dir_name_list_backup, nmbr_files_d1, nmbr_files_backup, dir_origem, dir_dest);
					}

					if (compare_time(start, finish) > 0 || fs)
					{
						exec_sync(origem, dest);
					}
				}
				j++;
			}
			i++;
		}
		fs = 0;
		sleep(5);
	}
}

void create_files_into_backup(dir_properties **namelist, dir_properties **namelist_backup, int number_of_files_dir, int number_of_files_dir_backup, char dir_origem[], char dir_dest[])
{
	int i = 2;
	int exec = 1;
	int force_exec = 1;

	while (i < number_of_files_dir)
	{
		if (number_of_files_dir_backup == 2 || number_of_files_dir_backup < number_of_files_dir  || strcmp(namelist[i]->d_name, namelist_backup[i]->d_name) != 0)
		{
			char concated_string[] = "minhapasta_backup/";
			char source[255];
			strcpy(source, namelist[i]->d_name);
			strcat(concated_string, source);
			FILE *out = fopen(concated_string, "w");
			fclose(out);
			number_of_files_dir_backup = scandir(dir_dest, &namelist_backup, NULL, alphasort);
			if (number_of_files_dir == number_of_files_dir_backup)
			{
				run_application(namelist, namelist_backup, number_of_files_dir, number_of_files_dir_backup, dir_origem, dir_dest, 1);
			}
		}
		i++;
	}
}

int main(int argc, char **argv)
{
	int number_of_files_dir;
	int number_of_files_dir_backup;

	char dir_origem[255] = "minhapasta/";
	char dir_dest[255] = "minhapasta_backup/";

	dir_properties **fileList;
	dir_properties **fileList_backup;

	number_of_files_dir = scandir(dir_origem, &fileList, NULL, alphasort);
	number_of_files_dir_backup = scandir(dir_dest, &fileList_backup, NULL, alphasort);
	
	if (number_of_files_dir == -1 || number_of_files_dir_backup == -1)
		printf("\nFALHA! Diretório Inexistente\n");

	if (number_of_files_dir != number_of_files_dir_backup)
	{
		create_files_into_backup(fileList, fileList_backup, number_of_files_dir, number_of_files_dir_backup, dir_origem, dir_dest);
	}

	else
	{
		run_application(fileList, fileList_backup, number_of_files_dir, number_of_files_dir_backup, dir_origem, dir_dest, 0);
	}
	return 0;
}