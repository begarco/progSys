#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>     
#include <time.h>       
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#define	NBFILS	4

int heure = 0;
int pere = 0;
int fils[NBFILS];

void display1(int signal);
void display2(int signal);
void display3(int signal);

int main() {
	time_t t[2];
	int choix = 1;
	int result = 0;
	int fd[3][2];
	pipe(fd[0]);
	pipe(fd[1]);
	pipe(fd[2]);
	
	pere = getpid();
	
	for(int i = 0; i  < NBFILS ; i++) {
		fils[i] = fork();
		if(!fils[i]) {	/// les fils
			if(i==0){ 		/// heures
				signal(SIGUSR1, display1);
				close(fd[0][1]);	// fermeture ecriture
				while(1) {
					read(fd[0][0], &result, sizeof(result));	//lecture
					heure++;
					if(heure>=2) {
						heure=0;
					}
					printf("H=%02d\n",heure);
					fflush(stdout);
				}
			} else if(i==1){	/// minutes
				signal(SIGUSR1, display2);
				heure=0;
				close(fd[0][0]);	// fermeture lecture
				close(fd[1][1]);	// fermeture ecriture
				while(1) {
					read(fd[1][0], &result, sizeof(result));	//lecture
					heure++;
					if(heure>=3) {
						write(fd[0][1], &result, sizeof(result));	//ecriture
						heure=0;
					}
					printf("M=%02d\n", heure);
					fflush(stdout);
				}
			} else if(i==2){	/// secondes
				signal(SIGUSR1, display3);
				heure=0;
				time(&t[0]);
				time(&t[1]);
				close(fd[1][0]);	// fermeture lecture
				while(1) {
					while(1.00 > difftime(t[1],t[0])) {
						time(&t[1]);
					}
					heure++;
					t[0] = t[1];
					if(heure>=3) {
						write(fd[1][1], &result, sizeof(result));	//ecriture
						heure=0;
					}
					printf("S=%02d\n",heure);
					fflush(stdout);
				}
			} else if(i==3) {
				while(choix) {
					puts("Horloge Suisse\n--------------\n");
					puts("1 - Afficher l'heure");
					puts("2 - Incrementer les secondes");
					puts("3 - Incrementer les minutes");
					puts("4 - Incrementer les heures");
					puts("5 - Reset");
					puts("0 - Quitter");
					fflush(stdout);
					scanf("%d", &choix);
		
					if(1==choix) {
						printf("Il est : ");
						fflush(stdout);
						//for(int i = 0 ; i < 3 ; ++i) {
							kill(fils[0], SIGUSR1);
							fflush(stdout);
							kill(fils[1], SIGUSR1);
							fflush(stdout);
							kill(fils[2], SIGUSR1);
							fflush(stdout);
						//}
					} else if(2==choix) {
						close(fd[0][0]);	// fermeture lecture
						close(fd[1][0]);	// fermeture lecture
						close(fd[2][0]);	// fermeture lecture
						result = 5;
						write(fd[1][1], &result, sizeof(result));	//ecriture
					} else if(3==choix) {
					
					} else if(4==choix) {
					
					} else if(5==choix) {
					
					} else if(0==choix) {
						kill(pere, SIGTERM);
						for(int j = 0 ; j < NBFILS ; j++) {
							kill(fils[j], SIGTERM);
						}
					}
				}
			}
			return 0;
		}
	}
	
	pause();
	
	return 0;
}


void display1(int signal) {
	printf("%02d:", heure);
}

void display2(int signal) {
	printf("%02d:", heure);
}

void display3(int signal) {
	printf("%02d\n", heure);
}


