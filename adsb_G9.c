#include <stdio.h>
#include <stdlib.h>

#include "ask.h"

int main(int argc, char* argv[]) {
#pragma GCC diagnostic ignored "-Wunused-result"
	srand((unsigned int)time(NULL));
	FILE* input_file = fopen(argv[1], "r");
	FILE* output_file = fopen(argv[2], "w");
	FILE* answer_file = fopen(argv[3], "r");

	if (!input_file || !output_file || !answer_file) {
		printf("error\n");
		exit(EXIT_FAILURE);
	}

	int p_ins, p_sub, p_del;
	fscanf(input_file, "%d %d %d", &p_ins, &p_sub, &p_del);

	char** S = (char**)malloc(sizeof(char*) * N);

	for (int i = 0; i < N; i++) {
		S[i] = (char*)malloc(sizeof(char) * (DATA_LENGTH + 1));
		fscanf(input_file, "%s", S[i]);
	}

	for (int i = 0; i < Q; i++) {
		char* q = (char*)malloc(sizeof(char) * 200);
		fscanf(input_file, "%s", q);

		q = ask(i + 1, argv[3]);

		int ans = randint(1, N + 1);
		fprintf(output_file, "%d\n", ans);

		free(q);
	}

	fclose(input_file);
	fclose(output_file);
	for (int i = 0; i < N; i++) {
		free(S[i]);
	}
	free(S);
#pragma GCC diagnostic warning "-Wunused-result"
	return 0;
}