#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ask.h"

void min_object(int *array, int num, int *min_index)
{
	int min = INT_MAX;
	*min_index = -1;
	for (int i = 0; i < num; i++)
	{
		if (array[i] < min)
		{
			min = array[i];
			*min_index = i;
		}
	}
}

// 最小の数を探す
int min(int a, int b, int c)
{
	return a > b ? (b > c ? c : b) : (a > c ? c : a);
}

// レーベンシュタイン距離を求める
int LevenshteinDistance(char *str1, char *str2)
{
	int lenstr1 = strlen(str1) + 1;
	int lenstr2 = strlen(str2) + 1;
	int d[lenstr1][lenstr2];
	int i1 = 0, i2 = 0, cost = 0;

	for (; i1 < lenstr1; i1++)
		d[i1][0] = i1;
	for (; i2 < lenstr2; i2++)
		d[0][i2] = i2;

	for (i1 = 1; i1 < lenstr1; i1++)
	{
		for (i2 = 1; i2 < lenstr2; i2++)
		{
			cost = str1[i1 - 1] == str2[i2 - 1] ? 0 : 1;
			d[i1][i2] = min(d[i1 - 1][i2] + 1, d[i1][i2 - 1] + 1, d[i1 - 1][i2 - 1] + cost);
		}
	}

	return d[lenstr1 - 1][lenstr2 - 1];
}

int PredictAnswer(char **S, char *q, int id, int *ans_dis, const int p_ins, const int p_sub, const int p_del)
{
	const int length = strlen(q);
	int ans_id = -1;
	const double entropy = (p_ins + p_sub + p_del) / 15.0;
	for (int id = 0; id < N; id++)
	{
		int min_dis = INT_MAX;
		for (int i = 0; i < DATA_LENGTH; i += length / (5.0 - entropy))
		{
			char *temp = (char *)malloc(sizeof(char) * (length + 1));
			strncpy(temp, S[id] + i, length);
			temp[length] = '\0';
			int dis = LevenshteinDistance(temp, q);
			if (dis < min_dis)
			{
				min_dis = dis;
				ans_id = id;
			}
			if (dis < length / 3) // 編集距離はQueryの長さにも依存するのでそれも考慮する
			{
				// 多分これであっているだろうから早とちりしてみる
				// printf("abort\n");
				return ans_id + 1;
			}
		}
		ans_dis[id] += min_dis;
	}
	// printf("dis=%d\n", ans_dis);
	return 0;
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

	// p_ins, p_sub, p_del
	for (int i = 0; i < Q; i++)
	{
		int ans = 0;
		int ans_dis[Q + 1] = {0};
		char *q = (char *)malloc(sizeof(char) * 100);
		fscanf(input_file, "%s", q);

		int t = PredictAnswer(S, q, i, ans_dis, p_ins, p_sub, p_del);
		free(q); // 予測したらもういらないよね
		if (t != 0)
		{
			ans = t;
		}
		else
		{
			int min_index;
			min_object(ans_dis, Q, &min_index);
			ans = min_index + 1;
		}

		fprintf(output_file, "%d\n", ans);
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