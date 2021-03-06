#include "image.h"
#include "pthread.h"

struct image input, output;
int rayon = 0, iteration = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void * fonction(void * args) {
	unsigned a = (unsigned*)args;
	for(int j = 0 ; j < iteration ; j++) {
		blur_image_row(&input, &output, rayon, a);
	}
}

int main(int arg_count, char **args) {
	clock_t start = clock(), end = 0;

	if(arg_count >= 4) {
		input = make_image_from_file(args[1]);
		output = make_image(input.type, input.row_count, input.column_count, input.max_value);
		pthread_t thread[input.row_count];
		rayon = strtoul(args[3], 0, 0);
		iteration = arg_count==5 ? strtoul(args[4], 0, 0) : 1;
		for(unsigned h = 0; h < input.row_count; h++) {
				pthread_create(&thread[h], 0, fonction, h);
		}

		for(int i = 0; i < input.row_count; ++i)
			pthread_join(thread[i], 0); // attente

		write_image_to_file(&output, args[2]);
	}
	else {
		fprintf(stderr, "Essaie plutôt : %s input.ppm output.ppm 10", args[0]);
	}
	end = clock();
	printf("Temps d'execution : %f\n", (double)(end-start)/CLOCKS_PER_SEC);
}
