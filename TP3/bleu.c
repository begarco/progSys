#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>     
#include <time.h>       
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

void f(int a) {
	puts("COUCOU");
	fflush(stdout);
}

int main() {
	printf("%d\n", getpid());
	while(1) {
		signal(SIGUSR1, f);
	}
	printf("Fin\n");
	return 0;
}
