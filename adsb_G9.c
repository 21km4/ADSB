#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>

#include "ask.h"

static int abort_count = 0;
static int ask_count = 0;

#pragma region BITPARALLEL
int weighted_levenshtein_bitpal(char *a, char *b, size_t len)
{
	if (len > 64)
	{
		return weighted_levenshtein_bitpal(a, b, 64) + weighted_levenshtein_bitpal(a + 64, b + 64, len - 64);
	}

	uint64_t posbits[256] = {0};

	for (int i = 0; i < len; i++)
	{
		posbits[(unsigned char)a[i]] |= 1ull << i;
	}

	uint64_t DHneg1 = ~0x0ull;
	uint64_t DHzero = 0;
	uint64_t DHpos1 = 0;

	// recursion
	for (int i = 0; i < len; ++i)
	{
		uint64_t Matches = posbits[(unsigned char)b[i]];
		// Complement Matches
		uint64_t NotMatches = ~Matches;

		// Finding the vertical values. //Find 1s
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

	int dist = len;

	for (int i = 0; i < len; i++)
	{
		uint64_t bitmask = 1ull << i;
		dist -= ((add1 & bitmask) >> i) * 1 + ((add2 & bitmask) >> i) * 2 - 1;
	}

	return dist;
}
#pragma endregion

int PredictAnswer(char **S, char *q, int id, const int p_ins, const int p_sub, const int p_del, const int length)
{
	int ans_id = -1;
	int min_dis = INT_MAX;
	for (int id = 0; id < N; id++)
	{
		for (int i = 0; i < DATA_LENGTH; i += length / 7.0)
		{
			static char temp[100 + 1];
			strncpy(temp, S[id] + i, length);
			temp[length] = '\0';
			int dis = weighted_levenshtein_bitpal(temp, q, length);
			if (dis < min_dis)
			{
				min_dis = dis;
				ans_id = id;
			}
			if (dis < length / 3)
			{
				abort_count++;
				return ans_id + 1;
			}
		}
	}
	return ans_id + 1;
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

	int p_ins, p_sub, p_del;
	fscanf(input_file, "%d %d %d", &p_ins, &p_sub, &p_del);

	char **S = (char **)malloc(sizeof(char *) * N);

	for (int i = 0; i < N; i++)
	{
		S[i] = (char *)malloc(sizeof(char) * (DATA_LENGTH + 1));
		fscanf(input_file, "%s", S[i]);
	}
#pragma endregion

	int answer;
	char *q;
	for (int i = 0; i < Q; i++)
	{
		q = malloc(sizeof(char) * Q);
		fscanf(input_file, "%s", q);
		// q = ask(i + 1, argv[3]);
		
		const int length = strlen(q);
		const int answer = PredictAnswer(S, q, i, p_ins, p_sub, p_del, length);
		free(q);
		fprintf(output_file, "%d\n", answer);

		// 信号長が短い場合にaskを呼んで正確な信号を得るのが良いだろう
		// つまりaskを呼び出す回数をqの長さで変化させるということ
		
	}
	printf("abort times: %d\n", abort_count);
	printf("ask times: %d\n", ask_count);

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
	return 0;
}