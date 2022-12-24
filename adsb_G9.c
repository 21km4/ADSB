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

#define max(a, b) ((a) > (b) ? (a) : (b))
#define swap(type, a, b) ({ type temp = (a); (a) = (b); (b) = (temp); })

int snake(int k, int y, const char *str1, const char *str2, const size_t str1_size, const size_t str2_size)
{
	int x = y - k;

	while (x < str1_size && y < str2_size && str1[x] == str2[y])
	{
		x++;
		y++;
	}

	return y;
}

#define SIZE 200
int edit_distance_onp(char *str1, char *str2)
{
	size_t str1_size = strlen(str1);
	size_t str2_size = strlen(str2);
	// required: str1_size <= str2_size
	if (str1_size > str2_size)
	{
		swap(char *, str1, str2);
		swap(size_t, str1_size, str2_size);
	}
	static int fp[SIZE];
	int x, y, k, p;
	const int offset = str1_size + 1;
	const int delta = str2_size - str1_size;
	for (int i = 0; i < SIZE; i++)
		fp[i] = -1;

	for (p = 0; fp[delta + offset] != str2_size; p++)
	{
		for (k = -p; k < delta; k++)
			fp[k + offset] = snake(k, max(fp[k - 1 + offset] + 1, fp[k + 1 + offset]), str1, str2, str1_size, str2_size);
		for (k = delta + p; k > delta; k--)
			fp[k + offset] = snake(k, max(fp[k - 1 + offset] + 1, fp[k + 1 + offset]), str1, str2, str1_size, str2_size);
		fp[delta + offset] = snake(delta, max(fp[delta - 1 + offset] + 1, fp[delta + 1 + offset]), str1, str2, str1_size, str2_size);
	}

	return delta + (p - 1) * 2;
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
			int dis = edit_distance_onp(temp, q);
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