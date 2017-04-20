#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NPT		10000
#define NLN		1000000

#define MIN(a, b)	((a) < (b) ? (a) : (b))

static int pt_x[NPT];
static int pt_y[NPT];
static int pt_n;
static int pt_min = 3;

/* test case input */
static int input(FILE *fp)
{
	int i;
	if (fscanf(fp, "%d", &pt_n) != 1)
		return -1;
	for (i = 0; i < pt_n; i++)
		if (fscanf(fp, "%d %d", &pt_x[i], &pt_y[i]) != 2)
			return -1;
	return 0;
}

static int collinear(int p0, int p1, int p2)
{
	return (pt_x[p0] - pt_x[p1]) * (long) (pt_y[p0] - pt_y[p2]) ==
		(pt_x[p0] - pt_x[p2]) * (long) (pt_y[p0] - pt_y[p1]);
}

static int pt_sorted[NPT];
static int pt_sx[NPT];
static int pt_sy[NPT];

#define DEGCMP(x1, y1, x2, y2)		(-x1 * (long) y2 + x2 * (long) y1)

static void pt_insertion(int *a, int n)
{
	int i, j;
	for (i = 0; i < n; i++) {
		int k = a[i];
		for (j = i - 1; j >= 0 && DEGCMP(pt_sx[k], pt_sy[k], pt_sx[a[j]], pt_sy[a[j]]) < 0; j--)
			a[j + 1] = a[j];
		a[j + 1] = k;
	}
}

static void pt_quicksort(int *a, int n)
{
	int i, j, t;
	int kx, ky;
	if (n < 2)
		return;
	if (n < 100) {
		pt_insertion(a, n);
		return;
	}
	kx = pt_sx[a[n / 2]];
	ky = pt_sy[a[n / 2]];
	for (i = 0, j = n - 1; ; i++, j--) {
		while (DEGCMP(pt_sx[a[i]], pt_sy[a[i]], kx, ky) < 0)
			i++;
		while (DEGCMP(pt_sx[a[j]], pt_sy[a[j]], kx, ky) > 0)
			j--;
		if (i >= j)
			break;
		t = a[i];
		a[i] = a[j];
		a[j] = t;
	}
	if (i > 1)
		pt_quicksort(a, i);
	if (i + 1 < n)
		pt_quicksort(a + i, n - i);
}

static void sortpoints(int base)
{
	int i;
	for (i = 0; i < pt_n; i++)
		if (pt_sorted[i] == base)
			break;
	pt_sorted[i] = pt_sorted[0];
	pt_sorted[0] = base;
	for (i = 0; i < pt_n; i++) {
		pt_sx[i] = pt_x[i] - pt_x[base];
		pt_sy[i] = pt_y[i] - pt_y[base];
		if (pt_sy[i] < 0 || (pt_sy[i] == 0 && pt_sx[i] > 0)) {
			pt_sx[i] = -pt_sx[i];
			pt_sy[i] = -pt_sy[i];
		}
	}
	pt_quicksort(pt_sorted + 1, pt_n - 1);
}

static int *ln[NLN];
static int ln_cnt[NLN];
static int ln_n;

static void line_add(int p0, int n, int *pt)
{
	ln[ln_n] = malloc((n + 1) * sizeof(ln[ln_n][0]));
	ln_cnt[ln_n] = n + 1;
	ln[ln_n][0] = p0;
	memcpy(ln[ln_n] + 1, pt, n * sizeof(ln[ln_n][0]));
	ln_n++;
}

static long pt_dist(int a, int b)
{
	long x = pt_x[a] - pt_x[b];
	long y = pt_y[a] - pt_y[b];
	return x * x + y * y;
}

static char pt_mark[NPT];

static int closestunmarked(int p)
{
	int ret = -1;
	long retdis = 0;
	int i;
	for (i = 0; i < pt_n; i++) {
		if (!pt_mark[i] && (ret < 0 || pt_dist(p, i) < retdis)) {
			ret = i;
			retdis = pt_dist(p, i);
		}
	}
	return ret;
}

static int lowerleft(void)
{
	int ret = 0;
	int i;
	for (i = 1; i < pt_n; i++)
		if (pt_y[i] < pt_y[ret] || (pt_y[i] == pt_y[ret] && pt_x[i] < pt_x[ret]))
			ret = i;
	return ret;
}

int main(int argc, char *argv[])
{
	int i, j;
	int pt;
	if (input(stdin))
		return 1;
	for (j = 0; j < pt_n; j++)
		pt_sorted[j] = j;
	pt = lowerleft();
	for (i = 0; i < pt_n - pt_min + 1; i++) {
		int cur = 1;
		sortpoints(pt);
		pt_mark[pt] = 1;
		while (cur < pt_n) {
			int beg = cur;
			int good = 1;
			while (cur < pt_n && collinear(pt, pt_sorted[beg], pt_sorted[cur]))
				cur++;
			if (cur - beg + 1 < pt_min)
				continue;
			for (j = beg; good && j < cur; j++)
				if (pt_mark[pt_sorted[j]])
					good = 0;
			if (good && cur - beg + 1 >= pt_min)
				line_add(pt, cur - beg, pt_sorted + beg);
		}
		pt = closestunmarked(pt);
	}
	printf("%d\n", ln_n);
	for (i = 0; i < ln_n; i++) {
		printf("%d", ln_cnt[i]);
		for (j = 0; j < ln_cnt[i]; j++)
			printf(" %d", ln[i][j]);
		printf("\n");
	}
	return 0;
}
