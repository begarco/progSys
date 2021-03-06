#define _XOPEN_SOURCE	500

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include <strings.h>
#include <string.h>

#include "file.h"

#define BUFFER_SIZE	1024

int isRunning = 1;
static file_t * users;

void stop(int id) {
	id += 0;
	isRunning = 0;
	unlink("./data/serveur");
	delete_file(users);
	puts("\n> Session closed.\n");
	fflush(stdout);
	exit(EXIT_SUCCESS);
}

int main() {
	
	int fd = 0, keep_open = 0;
	char buff[BUFFER_SIZE] = {0};

	users = create_file();

	signal(SIGINT, stop);
	unlink("./data/serveur");
	if(mkfifo("./data/serveur", 0600) != 0) {
		perror("Erreur sur la creation du tube du serveur ");
		exit(EXIT_FAILURE);
	}

	puts("###########################################");
	puts("#                BAVARDAGE                #");
	puts("###########################################");
	puts("\n");
	puts("# New server session started.");
	fflush(stdout);

	int result = 0;
	keep_open = open("serveur", O_WRONLY | O_NONBLOCK);
	write(fd, &result, sizeof(result));
	fd = open("./data/serveur", O_RDONLY);

	while(isRunning) {
		if((result = read(fd, buff, sizeof(buff))) == -1)
			perror("Erreur ");
		if(result > 0) {
			char type = buff[0];
			int taille = buff[1];
			char name[50];
			char message[500];
			bzero(name, sizeof(name));
			bzero(message, sizeof(message));
			strcpy(name, &(buff[2+taille]));
		
			if(type=='M') {	/* reception d'un message */
				strncpy(message, &(buff[2]), taille);
				printf("> %s dit : %s\n", name, message);
				char buffer[512] = {0};
				sprintf(buffer, "> %s dit : %s\n", name, message);
				for(elt_t * cur = users->first ; cur != NULL ; cur = cur->next) {
					if(strcmp(name, cur->name)) {
						char path[64] = "./data/";
						strcat(path, cur->name);
						int comm = open(path, O_WRONLY);
						write(comm, buffer, sizeof(buffer));
						close(comm);
						//printf("Send to %s.\n", cur->name);
					}
				}
			} else if(type=='C') {	/* connexion */
				if(!contains(users, name)) {
					char path[64] = "./data/";
					strcat(path, name);
					unlink(path);
					mkfifo(path, 0600);
					push_file(users, name, path);
					printf("# New connexion: %s. Number of users: %d.\n", name, users->size);
				}
			} else if(type=='D') {	/* deconnexion */
				elt_t * toRemove = remove_file(users, name);
				printf("# Connexion closed: %s. Number of users: %d.\n", toRemove->name, users->size);
				free(toRemove->name);
				unlink(toRemove->fich);
				free(toRemove->fich);
				free(toRemove);
			}
			fflush(stdout);
		}
	}


	return 0;
}
