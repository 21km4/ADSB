#include <stdio.h>
#include <stdlib.h>

#include "ask.h"

int DecideTrials(const int p_ins, const int p_sub, const int p_del) {
	return 5;
}

void RestoreSignal(char** qs, char* r_signal, const int q_trials) {
	for (int i = 0; i < 200; i++){
		if (qs[0][i] != '\0') {
			r_signal[i] = '\0';
			break;
		}
		for (int j = 0; j < q_trials; j++) {

		}
	}
}

int main(int argc, char *argv[])
{
#pragma GCC diagnostic ignored "-Wunused-result"
#pragma region INITIALIZE
	srand((unsigned int)time(NULL));
	FILE *input_file = fopen(argv[1], "r");
	FILE *output_file = fopen(argv[2], "w");
	FILE *answer_file = fopen(argv[3], "r");

	if (!input_file || !output_file || !answer_file)
	{
		printf("error\n");
		exit(EXIT_FAILURE);
	}

	int p_ins, p_sub, p_del; // Insert, Substitute, Delete
	fscanf(input_file, "%d %d %d", &p_ins, &p_sub, &p_del);

	char **S = (char **)malloc(sizeof(char *) * N); // Alloc heap memory

	for (int i = 0; i < N; i++)
	{
		S[i] = (char *)malloc(sizeof(char) * (DATA_LENGTH + 1));
		fscanf(input_file, "%s", S[i]);
	}
#pragma endregion

	const int q_trials = DecideTrials(p_ins, p_sub, p_del);

	// p_ins, p_sub, p_del
	for (int i = 0; i < Q; i++)
	{
		char **qs = (char **)malloc(sizeof(char *) * q_trials);
		qs[0] = (char *)malloc(sizeof(char) * 200);
		fscanf(input_file, "%s", qs[0]);
		for (int j = 1; j < q_trials; j++)
		{
			qs[j] = (char *)malloc(sizeof(char) * 200);
			qs[j] = ask(i + 1, argv[3]); // Query
		}
		
		char* r_signal = (char* )malloc(sizeof(char) * 200);
		RestoreSignal(qs, r_signal, q_trials);

		int ans = 0; // diff to S[i]

		fprintf(output_file, "%d\n", ans);
		for (int i = 0; i < q_trials; i++)
			free(qs[i]);
		free(qs);
	}

#pragma region FINALIZE
	fclose(input_file);
	fclose(output_file);
	for (int i = 0; i < N; i++)
	{
		free(S[i]);
	}
	free(S);
#pragma endregion
#pragma GCC diagnostic warning "-Wunused-result"
	return 0;
}