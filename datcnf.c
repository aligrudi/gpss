#include <stdio.h>
#include <stdlib.h>

/* Pappus configuration */
static void test2(void)
{
	printf("9 9\n");
	printf("3 0 1 2\n");
	printf("3 3 4 5\n");
	printf("3 6 7 8\n");
	printf("3 0 3 7\n");
	printf("3 0 4 8\n");
	printf("3 1 3 6\n");
	printf("3 1 5 8\n");
	printf("3 2 4 6\n");
	printf("3 2 5 7\n");
}

/* modified Pappus configuration */
static void test3(void)
{
	printf("9 8\n");
	printf("3 0 1 2\n");
	printf("3 3 4 5\n");
	printf("3 6 7 8\n");
	printf("3 0 3 7\n");
	printf("3 0 4 8\n");
	printf("3 1 3 6\n");
	printf("3 2 4 6\n");
	printf("3 2 5 7\n");
}

/* Fano plane */
static void test4(void)
{
	printf("7 7\n");
	printf("3 0 1 2\n");
	printf("3 1 3 5\n");
	printf("3 0 3 4\n");
	printf("3 0 5 6\n");
	printf("3 1 4 6\n");
	printf("3 2 3 6\n");
	printf("3 2 4 5\n");
}

/* 3-chain */
static void test5(void)
{
	int n = 1000 & ~1;
	int l = 3;
	int i, j;
	printf("%d %d\n", n, n / 2);
	for (i = 0; i < n; i += 2) {
		printf("%d", l);
		for (j = 0; j < l; j++)
			printf(" %d", (i + j) % n);
		printf("\n");
	}
}

int main(int argc, char *argv[])
{
	int id = atoi(argv[1]);
	if (id == 0)
		test2();
	if (id == 1)
		test3();
	if (id == 2)
		test4();
	if (id == 3)
		test5();
	return 0;
}
