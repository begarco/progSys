#include "image.h"
#include "pthread.h"

struct image input, output;
int rayon = 0, iteration = 0;;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void para_blur_image(struct image const *input, struct image *output, unsigned radius, unsigned layer) {
	FOR_EACH_OUTPUT_ROW(
		FOR_EACH_OUTPUT_COLUMN(
			unsigned sum = 0;

			//pthread_mutex_lock(&mutex); // verrouillage

			for(unsigned input_row = input_row_begin; input_row < input_row_end; ++input_row) {
				for(unsigned input_column = input_column_begin; input_column < input_column_end; ++input_column) {
					sum += (unsigned) input->data.as_rgb8[input_row * input->column_count + input_column][layer];
				}
			}
			output->data.as_rgb8[output_row * output->column_count + output_column][layer] = (unsigned char)(sum / input_row_count / input_column_count);

			//pthread_mutex_unlock(&mutex); // déverrouillage
		)
	)
}

void * fonction(void * args) {
	for(int j = 0 ; j < iteration ; j++)
		para_blur_image(&input, &output, rayon, (int*)args);
}

int main(int arg_count, char **args) {
	pthread_t thread[3];
	clock_t start = clock(), end = 0;

	if(arg_count >= 4) {
		input = make_image_from_file(args[1]);
		output = make_image(input.type, input.row_count, input.column_count, input.max_value);
		rayon = strtoul(args[3], 0, 0);
		iteration = arg_count==5 ? strtoul(args[4], 0, 0) : 1;
		for(int i = 0 ; i < 3 ; ++i) {
			pthread_create(&thread[i], 0, fonction, i);
		}

		for(int i = 0; i < 3; ++i)
			pthread_join(thread[i], 0); // attente
		
		write_image_to_file(&output, args[2]);
	}
	else {
		fprintf(stderr, "Essaie plutôt : %s input.ppm output.ppm 10", args[0]);
	}
	end = clock();
	printf("Temps d'execution : %f\n", (double)(end-start)/CLOCKS_PER_SEC);
}
