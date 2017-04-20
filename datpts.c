#include <stdio.h>
#include <stdlib.h>

/* generate an hxw grid */
static void grid(int h, int w)
{
	int i, j;
	printf("%d\n", h * w);
	for (i = 0; i < h; i++)
		for (j = 0; j < w; j++)
			printf("%d %d\n", i, j);
}

/* two hxw grids */
static void grid2(int h, int w, int dy, int dx)
{
	int i, j;
	printf("%d\n", h * w * 2);
	for (i = 0; i < h; i++)
		for (j = 0; j < w; j++)
			printf("%d %d\n", i, j);
	for (i = 0; i < h; i++)
		for (j = 0; j < w; j++)
			printf("%d %d\n", dy + i, dx + j);
}

int main(int argc, char *argv[])
{
	int id = atoi(argv[1]);
	if (id == 0)
		grid(10, 10);
	if (id == 1)
		grid(13, 13);
	if (id == 2)
		grid(14, 14);
	if (id == 3)
		grid(15, 15);
	if (id == 4)
		grid(17, 17);
	if (id == 5)
		grid(5, 22);
	if (id == 6)
		grid(3, 37);
	if (id == 7)
		grid2(13, 13, 5, 0);
	if (id == 8)
		grid2(12, 12, 7, 7);
	return 0;
}
