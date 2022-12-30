#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>
#ifdef EVALUATE_MODE
#include <time.h>
#endif

#include "ask.h"

#define EVALUATE_MODE

static int p_ins, p_sub, p_del;
static char **S;
static char *q;

#ifdef EVALUATE_MODE
static int abort_count = 0;
static int ask_count = 0;
static int compute_time;

void evaluate(char *argv[])
{
#pragma GCC diagnostic ignored "-Wunused-result"
	FILE *output_file = fopen(argv[2], "r");
	FILE *answer_file = fopen(argv[3], "r");

	int p_ins, p_sub, p_del;
	fscanf(answer_file, "%d %d %d", &p_ins, &p_sub, &p_del);

	int correct = 0, i;
	for (i = 0; i < N; i++)
	{
		int input[N], answer[N];
		char data[N];
		if (fscanf(output_file, "%d", &input[i]) == EOF)
			break;
		fscanf(answer_file, "%d %s", &answer[i], data);
		if (input[i] == answer[i])
			correct++;
	}

	// printf("abort times: %d\n", abort_count);
	// printf("ask times: %d\n", ask_count);

	printf("%d/%d Correct.\n", correct, i);
	printf("Score: %d\n", correct * 100 - ask_count * 5);
	printf("Time: %lf seconds\n", (double)compute_time / CLOCKS_PER_SEC);
	printf("\n");

	fclose(output_file);
	fclose(answer_file);
#pragma GCC diagnostic warning "-Wunused-result"
}
#endif

#pragma region BITPARALLEL
int weighted_levenshtein_bitpal(char *a, char len_a, char *b, int len_b)
{
	if (len_a > 64)
	{
		return weighted_levenshtein_bitpal(a, 64, b, len_b > 64 ? 64 : len_b);
	}

	uint64_t posbits[256] = {0};

	for (int i = 0; i < len_a; i++)
	{
		posbits[(unsigned char)a[i]] |= 1ull << i;
	}

	uint64_t DHneg1 = ~0x0ull;
	uint64_t DHzero = 0;
	uint64_t DHpos1 = 0;

	// recursion
	for (int i = 0; i < len_b; i++)
	{
		uint64_t Matches = posbits[(unsigned char)b[i]];
		// Complement Matches
		uint64_t NotMatches = ~Matches;

		// Finding the vertical values.
		// Find 1s
		uint64_t INITpos1s = DHneg1 & Matches;
		uint64_t DVpos1shift = (((INITpos1s + DHneg1) ^ DHneg1) ^ INITpos1s);

		// set RemainingDHneg1
		uint64_t RemainDHneg1 = DHneg1 ^ (DVpos1shift >> 1);
		// combine 1s and Matches
		uint64_t DVpos1shiftorMatch = DVpos1shift | Matches;

		// Find 0s
		uint64_t INITzeros = (DHzero & DVpos1shiftorMatch);
		uint64_t DVzeroshift = ((INITzeros << 1) + RemainDHneg1) ^ RemainDHneg1;

		// Find -1s
		uint64_t DVneg1shift = ~(DVpos1shift | DVzeroshift);
		DHzero &= NotMatches;
		// combine 1s and Matches
		uint64_t DHpos1orMatch = DHpos1 | Matches;
		// Find 0s
		DHzero = (DVzeroshift & DHpos1orMatch) | (DVneg1shift & DHzero);
		// Find 1s
		DHpos1 = (DVneg1shift & DHpos1orMatch);
		// Find -1s
		DHneg1 = ~(DHzero | DHpos1);
	}
	// find scores in last row
	uint64_t add1 = DHzero;
	uint64_t add2 = DHpos1;

	int dist = len_b;

	for (int i = 0; i < len_a; i++)
	{
		uint64_t bitmask = 1ull << i;
		dist -= ((add1 & bitmask) >> i) * 1 + ((add2 & bitmask) >> i) * 2 - 1;
	}

	return dist;
}
#pragma endregion

int predict_answer(const int index, char *answer_file, const int length)
{
	int ans_id = -1;
	int min_distance = INT_MAX;
	int multiple = 0;
	for (int id = 0; id < N; id++)
	{
		for (int i = 0; i < DATA_LENGTH; i += length / 10.0)
		{
			static char temp[N + 1];
			strncpy(temp, S[id] + i, length);
			temp[length] = '\0';
			int distance = weighted_levenshtein_bitpal(temp, length, q, length);
			if (distance < min_distance)
			{
				min_distance = distance;
				ans_id = id;
				multiple = 0;
			}
			if (distance == min_distance && ans_id != id)
			{
				multiple++;
			}
			if (distance < length / 4.0)
			{
#ifdef EVALUATE_MODE
				abort_count++;
#endif
				return ans_id + 1;
			}
		}
	}
	if (multiple)
	{
		free(q);
#ifdef EVALUATE_MODE
		ask_count++;
#endif
		q = ask(index + 1, answer_file);
		return predict_answer(index, answer_file, strlen(q) + 1);
	}
	return ans_id + 1;
}

int main(int argc, char *argv[])
{
#pragma GCC diagnostic ignored "-Wunused-result"
#pragma region INITIALIZE
#ifdef EVALUATE_MODE
	compute_time = clock();
#endif
	srand((unsigned int)time(NULL));
	FILE *input_file = fopen(argv[1], "r");
	FILE *output_file = fopen(argv[2], "w");
	FILE *answer_file = fopen(argv[3], "r");

	if (!input_file || !output_file || !answer_file)
	{
		fprintf(stderr, "error\n");
		exit(EXIT_FAILURE);
	}

	fscanf(input_file, "%d %d %d", &p_ins, &p_sub, &p_del);

	S = (char **)malloc(sizeof(char *) * N);

	for (int i = 0; i < N; i++)
	{
		S[i] = (char *)malloc(sizeof(char) * (DATA_LENGTH + 1));
		fscanf(input_file, "%s", S[i]);
		
	}
#pragma endregion

	for (int i = 0; i < Q; i++)
	{
		q = malloc(sizeof(char) * (N + 1));
		fscanf(input_file, "%s", q);
		int length = strlen(q) + 1;

		int answer = predict_answer(i, argv[3], length);
		free(q);

		fprintf(output_file, "%d\n", answer);
	}

#pragma region FINALIZE
	fclose(input_file);
	fclose(output_file);
	fclose(answer_file);
	for (int i = 0; i < N; i++)
	{
		free(S[i]);
	}
	free(S);
#pragma endregion
#pragma GCC diagnostic warning "-Wunused-result"

#ifdef EVALUATE_MODE
	compute_time = clock() - compute_time;
	evaluate(argv);
#endif

	return 0;
}