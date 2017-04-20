/* Generate random linear hypergraphs */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define NPT	1000
#define NLN	10000

static char ln_mark[NLN][NLN];
static short ln_point[NLN][NPT];
static int ln_cnt[NLN];
static short pt_line[NPT][NLN];
static int pt_cnt[NPT];
static int ln_n;
static int pt_n;

static void pt_init(int nln, int npt)
{
	ln_n = nln;
	pt_n = npt;
}

static void pt_put(int pt, int ln)
{
	int i;
	for (i = 0; i < pt_cnt[pt]; i++) {
		ln_mark[pt_line[pt][i]][ln] = 1;
		ln_mark[ln][pt_line[pt][i]] = 1;
	}
	pt_line[pt][pt_cnt[pt]++] = ln;
	ln_point[ln][ln_cnt[ln]++] = pt;
}

static int pt_collides(int pt, int ln)
{
	int i;
	for (i = 0; i < pt_cnt[pt]; i++)
		if (pt_line[pt][i] == ln || ln_mark[ln][pt_line[pt][i]])
			return 1;
	return 0;
}

static void pt_print(void)
{
	int i, j;
	int n = 0;
	for (i = 0; i < ln_n; i++)
		if (ln_cnt[i] > 2)
			n++;
	printf("%d\n", n);
	for (i = 0; i < ln_n; i++) {
		if (ln_cnt[i] > 2) {
			printf("%d", ln_cnt[i]);
			for (j = 0; j < ln_cnt[i]; j++)
				printf(" %d", ln_point[i][j]);
			printf("\n");
		}
	}
}

/* random lines and intersections */
static void testrand(int cnt)
{
	int i;
	for (i = 0; i < cnt * pt_n; i++) {
		int p = rand() % pt_n;
		int l = rand() % ln_n;
		if (!pt_collides(p, l))
			pt_put(p, l);
	}
}

int main(int argc, char *argv[])
{
	int npt = 100;
	int nln = 100;
	int cnt = 3;
	int seed = 0;
	int i;
	for (i = 1; i < argc && argv[i][0] == '-'; i++) {
		if (argv[i][1] == 's')
			seed = atoi(argv[i][2] ? argv[i] + 2 : argv[++i]);
	}
	if (i < argc)
		npt = atoi(argv[i]);
	if (i + 1 < argc)
		nln = atoi(argv[i + 1]);
	if (i + 2 < argc)
		cnt = atoi(argv[i + 2]);
	srand(seed);
	pt_init(nln, npt);
	testrand(cnt);
	pt_print();
	return 0;
}
