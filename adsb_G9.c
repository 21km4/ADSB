#define EVALUATE_MODE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef EVALUATE_MODE
#include <time.h>
#endif

#include "ask.h"

#ifndef INT_MAX
#define INT_MAX 0x7fffffff
#endif

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

	unsigned long long posbits[256] = {0};

	for (int i = 0; i < len_a; i++)
	{
		posbits[(unsigned char)a[i]] |= 1ull << i;
	}

	unsigned long long DHneg1 = ~0x0ull;
	unsigned long long DHzero = 0;
	unsigned long long DHpos1 = 0;

	// recursion
	for (int i = 0; i < len_b; i++)
	{
		unsigned long long Matches = posbits[(unsigned char)b[i]];
		// Complement Matches
		unsigned long long NotMatches = ~Matches;

		// Finding the vertical values.
		// Find 1s
		unsigned long long INITpos1s = DHneg1 & Matches;
		unsigned long long DVpos1shift = (((INITpos1s + DHneg1) ^ DHneg1) ^ INITpos1s);

		// set RemainingDHneg1
		unsigned long long RemainDHneg1 = DHneg1 ^ (DVpos1shift >> 1);
		// combine 1s and Matches
		unsigned long long DVpos1shiftorMatch = DVpos1shift | Matches;

		// Find 0s
		unsigned long long INITzeros = (DHzero & DVpos1shiftorMatch);
		unsigned long long DVzeroshift = ((INITzeros << 1) + RemainDHneg1) ^ RemainDHneg1;

		// Find -1s
		unsigned long long DVneg1shift = ~(DVpos1shift | DVzeroshift);
		DHzero &= NotMatches;
		// combine 1s and Matches
		unsigned long long DHpos1orMatch = DHpos1 | Matches;
		// Find 0s
		DHzero = (DVzeroshift & DHpos1orMatch) | (DVneg1shift & DHzero);
		// Find 1s
		DHpos1 = (DVneg1shift & DHpos1orMatch);
		// Find -1s
		DHneg1 = ~(DHzero | DHpos1);
	}
	// find scores in last row
	unsigned long long add1 = DHzero;
	unsigned long long add2 = DHpos1;

	int dist = len_b;

	for (int i = 0; i < len_a; i++)
	{
		unsigned long long bitmask = 1ull << i;
		dist -= ((add1 & bitmask) >> i) * 1 + ((add2 & bitmask) >> i) * 2 - 1;
	}

	return dist;
}
#pragma endregion

int predict_answer(const int index, char *answer_file, const int length, int *ids, const int k)
{
	int ans_id = -1;
	int min_distance = INT_MAX;
	int multiple = 0;
	int ans_ids[N] = {0};
	const int step = length / 10.0;
	for (int j = 0; j < k; j++)
	{
		int id = ids[j];
		for (int i = 0; i < DATA_LENGTH; i += step)
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
				ans_ids[0] = id;
			}
			if (distance == min_distance && ans_id != id)
			{
				if (ans_ids[multiple] != id)
				{
					multiple++;
					ans_ids[multiple] = id;
				}
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
		return predict_answer(index, answer_file, strlen(q) + 1, ans_ids, multiple + 1);
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
	int ids[N];
	for (int i = 0; i < N; i++)
	{
		ids[i] = i;
	}

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

		int answer = predict_answer(i, argv[3], length, ids, N);
		free(q);

		fprintf(output_file, "%d\n", answer);
	}

#pragma region FINALIZE
	fclose(input_file);
	fprintf(output_file, "%lf\n", clock() / (double)CLOCKS_PER_SEC);
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