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
#include <sys/file.h>
#include <errno.h>
#include <strings.h>
#include <string.h>

#define BUFFER_SIZE	1024

int isRunning = 1;
int pere, fils, serveur, ecoute, status = 0;
char filename[128] = "./data/";
char name[33] = {0};

void stop(int id) {
	id += 0;
	isRunning = 0;
	char toSend[500] = {0};
	bzero(toSend, sizeof(toSend));
	sprintf(toSend, "D%c%s", (char)1, name);
	write(serveur, '\0', 1);
	write(serveur, toSend, sizeof(toSend));
	usleep(50);
	close(serveur);
	close(ecoute);
	fflush(stdout);
	exit(EXIT_SUCCESS);
}

void fermer(int id) {
	id += 0;
	isRunning = 0;
	fflush(stdout);
	close(serveur);
	close(ecoute);
	puts("\n# Session closed.");
	fflush(stdout);
	exit(EXIT_SUCCESS);
}

int main(int arg_count, char ** args) {
	pere = getpid();

	puts("###########################################");
	puts("#                BAVARDAGE                #");
	puts("###########################################");
	puts("\n");

	if(arg_count == 2) {
		strncpy(name, args[1], 32);
		strcat(filename, name);

		serveur = open("./data/serveur", O_WRONLY | O_NONBLOCK);
		if(serveur != -1) {
			puts("# New client session started.");
			fflush(stdout);
			fils = fork();
			char buffer[BUFFER_SIZE] = {0};
			char bufferToSend[BUFFER_SIZE] = {0};
			bzero(buffer, sizeof(buffer));
			bzero(bufferToSend, sizeof(bufferToSend));

			/* connexion */
			sprintf(buffer, "C%c%s", 0, name);
			write(serveur, buffer, sizeof(buffer));
			bzero(buffer, sizeof(buffer));

			if(fils) {
				signal(SIGINT, stop);
				while(isRunning) {
					int nbChar = 0; char c = (char)0;
					while((c=getchar()) != '\n') {
						buffer[nbChar++] = c;
					}
					if(nbChar>0) {
						nbChar = (nbChar>255) ? 255 : nbChar;
						bufferToSend[0] = 'M';
						bufferToSend[1] = (char)nbChar;
						if(buffer[0] == 'D' && nbChar == 1) {
							bufferToSend[0] = 'D';
							isRunning = 0;
						}
						strncat(bufferToSend, buffer, 255);
						strncat(bufferToSend, name, 32);
						//printf("# %s\n", bufferToSend);
						flock(serveur, LOCK_EX);
						write(serveur, bufferToSend, sizeof(bufferToSend));
						flock(serveur, LOCK_UN);
					}
					bzero(buffer, sizeof(buffer));
					bzero(bufferToSend, sizeof(bufferToSend));
				}
				printf("# Stopping ...");
				fflush(stdout);
				kill(fils, SIGUSR1);
			} else {
				ecoute = -1;
				signal(SIGINT, fermer);
				signal(SIGUSR1, fermer);
				while((ecoute = open(filename, O_RDONLY | O_NONBLOCK)) == -1) {
					//perror("Cannot listen to the server ");
					usleep(10);
				}
				while(isRunning) {
					int taille = 0;
					bzero(buffer, sizeof(buffer));
					if((taille = read(ecoute, buffer, sizeof(buffer))) != -1) {
						printf("%s", buffer);
					}
				}
			}
		} else {
			perror("# Impossible de contacter le serveur ");
		}
	} else {
		puts("Erreur d'utilisation de la commande.");
	}

	waitpid(fils, &status, 0);

	return 0;
}
