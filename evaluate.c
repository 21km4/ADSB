#include <stdio.h>

int main(int argc, char* argv[]) {
    FILE *input_file = fopen(argv[1], "r");
	FILE *answer_file = fopen(argv[2], "r");

    int p_ins, p_sub, p_del; // Insert, Substitute, Delete
    fscanf(answer_file, "%d %d %d", &p_ins, &p_sub, &p_del);

    int correct = 0, i;
    for (i = 0; i < 100; i++) {
        int input[100], answer[100];
        char data[100];
        if (fscanf(input_file, "%d", &input[i]) == EOF) break;
        fscanf(answer_file, "%d %s", &answer[i], data);
        if (input[i] == answer[i]) correct++;
        printf("%d %d\t%s\n", input[i], answer[i], input[i] == answer[i] ? "OK" : "NG");
    }
    printf("\n%d/%d Correct.\n", correct, i);

    fclose(input_file);
	fclose(answer_file);
}