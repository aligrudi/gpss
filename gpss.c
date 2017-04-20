/* Algorithms for General Position Subset Selection */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NPT	10000
#define NLN	500000

static int **ln;		/* input lines */
static int *ln_cnt;		/* line point count */
static int ln_n;		/* number of lines */
static int **pt_ln;		/* lines a point is present in */
static int *pt_cnt;		/* number of lines a point is present in */
static int pt_n;		/* number of points */

/* read input data */
static int data_read(FILE *fp)
{
	int i, j;
	int pt_i[NPT] = {0};
	if (fscanf(fp, "%d", &ln_n) != 1)
		return -1;
	ln = malloc(ln_n * sizeof(ln[0]));
	ln_cnt = malloc(ln_n * sizeof(ln_cnt[0]));
	for (i = 0; i < ln_n; i++) {
		fscanf(fp, "%d", &ln_cnt[i]);
		ln[i] = malloc(ln_cnt[i] * sizeof(ln[i][0]));
		for (j = 0; j < ln_cnt[i]; j++)
			if (fscanf(fp, "%d", &ln[i][j]) != 1)
				return -1;
	}
	pt_ln = malloc(NPT * sizeof(pt_ln[0]));
	pt_cnt = malloc(NPT * sizeof(pt_cnt[0]));
	for (i = 0; i < ln_n; i++)
		for (j = 0; j < ln_cnt[i]; j++)
			pt_cnt[ln[i][j]]++;
	for (i = 0; i < NPT; i++)
		if (pt_cnt[i])
			pt_n = i + 1;
	for (i = 0; i < pt_n; i++)
		if (pt_cnt[i])
			pt_ln[i] = malloc(pt_cnt[i] * sizeof(pt_ln[i][0]));
	for (i = 0; i < ln_n; i++)
		for (j = 0; j < ln_cnt[i]; j++)
			pt_ln[ln[i][j]][pt_i[ln[i][j]]++] = i;
	return 0;
}

static void data_free(void)
{
	int i;
	for (i = 0; i < ln_n; i++)
		free(ln[i]);
	for (i = 0; i < pt_n; i++)
		free(pt_ln[i]);
	free(ln);
	free(pt_ln);
}

/* called by gpss_linecover(): find the next line to cover */
static int gpss_linecover_next(int *P, int *lnmk)
{
	int *lncovers;
	int i, j;
	int lnbest = -1;
	lncovers = calloc(ln_n, sizeof(lncovers[0]));
	for (i = 0; i < pt_n; i++) {
		int covered = 0;
		for (j = 0; j < pt_cnt[i]; j++)
			if (lnmk[pt_ln[i][j]])
				covered++;
		if (!covered && pt_cnt[i] == 1) {	/* point covered by only one line */
			for (j = 0; j < pt_cnt[i]; j++)
				if (!lnmk[pt_ln[i][j]])
					break;
			return pt_ln[i][j];
		}
		if (!covered)
			for (j = 0; j < pt_cnt[i]; j++)
				lncovers[pt_ln[i][j]]++;
	}
	for (i = 0; i < ln_n; i++)
		if (lncovers[i] > 0 && (lnbest < 0 || lncovers[i] >= lncovers[lnbest]))
			lnbest = i;
	free(lncovers);
	return lnbest;
}

/* obtain an upper bound for GPSS, based on an approximate line-cover */
static int gpss_linecover(int *S, int *P)
{
	int i, j;
	int S_n = 0;
	int S_n2 = 0;
	int *lnmk;	/* indicates if each line is selected */
	int *ptmk;	/* number of times a point is covered */
	int rem = 0;
	lnmk = calloc(ln_n, sizeof(lnmk[0]));
	ptmk = calloc(pt_n, sizeof(ptmk[0]));
	while (1) {
		int lnbest = gpss_linecover_next(P, lnmk);
		if (lnbest < 0)
			break;
		lnmk[lnbest] = 1;
		S[S_n++] = lnbest;
		/* marking covered points */
		for (i = 0; i < ln_cnt[lnbest]; i++)
			ptmk[ln[lnbest][i]] = 1;
	}
	/* removing lines covered twice */
	for (i = 0; i < S_n; i++) {
		int covered = 1;
		for (j = 0; j < ln_cnt[S[i]]; j++)
			if (ptmk[ln[S[i]][j]] == 1)
				covered = 0;
		if (covered) {
			for (j = 0; j < ln_cnt[S[i]]; j++)
				ptmk[ln[S[i]][j]]--;
		} else {
			S[S_n2++] = S[i];
		}
	}
	S_n = S_n2;
	/* uncovered vertices */
	for (i = 0; i < pt_n; i++)
		if (!ptmk[i])
			rem++;
	free(lnmk);
	free(ptmk);
	return S_n * 2 + rem;
}

/* called by gpss_greedy_cont(): find the next point to select */
static int gpss_greedy_next(int *P, int *ptmk)
{
	int i, j, k;
	for (i = 0; i < pt_n; i++) {
		int p = P[i];
		if (ptmk[p])
			continue;
		for (j = 0; j < pt_cnt[p]; j++) {
			int l = pt_ln[p][j];
			int lcnt = 0;
			for (k = 0; k < ln_cnt[l]; k++)
				if (ptmk[ln[l][k]])
					lcnt++;
			if (lcnt >= 2)
				break;
		}
		if (j == pt_cnt[p])
			return p;
	}
	return -1;
}

/* greedily find a subset in general position; add vertices in the given order */
static int gpss_greedy(int *S, int *P)
{
	int *ptmk;	/* indicates if each point is selected */
	int S_n = 0;
	int i;
	ptmk = calloc(pt_n, sizeof(ptmk[0]));
	for (i = 0; i < pt_n; i++) {
		int p = gpss_greedy_next(P, ptmk);
		if (p < 0)
			break;
		ptmk[p] = 1;
		S[S_n++] = p;
	}
	free(ptmk);
	return S_n;
}

/* called by gpss_greedymin_cont(): find the next point to select with the minimum number of collinear points */
static int gpss_greedymin_next(int *P, int *ptmk)
{
	int *lncnt;	/* number of selected points on each line */
	int *ptcnt;	/* number of remaining collinear points for each point */
	int *ptout;	/* indicates if point cannot be selected */
	int i, j, k;
	int best = -1;
	lncnt = calloc(ln_n, sizeof(lncnt[0]));
	ptcnt = calloc(pt_n, sizeof(ptcnt[0]));
	ptout = calloc(pt_n, sizeof(ptout[0]));
	for (i = 0; i < pt_n; i++) {
		if (ptmk[i]) {
			for (j = 0; j < pt_cnt[i]; j++)
				lncnt[pt_ln[i][j]]++;
		}
	}
	for (i = 0; i < pt_n; i++) {
		for (j = 0; j < pt_cnt[i]; j++)
			if (lncnt[pt_ln[i][j]] >= 2)
				ptout[i] = 1;
		if (ptout[i])
			continue;
		for (j = 0; j < pt_cnt[i]; j++) {
			int l = pt_ln[i][j];
			for (k = 0; k < ln_cnt[l]; k++)
				ptcnt[ln[l][k]]++;
		}
	}
	for (i = 0; i < pt_n; i++) {
		int p = P[i];
		if (!ptmk[p] && !ptout[p] && (best < 0 || ptcnt[p] < ptcnt[best]))
			best = p;
	}
	for (i = 0; i < pt_n; i++) {	/* points covered by only one line */
		int p = P[i];
		if (!ptmk[p] && !ptout[p] && pt_cnt[p] == 1)
			return p;
	}
	free(lncnt);
	free(ptcnt);
	free(ptout);
	return best;
}

/* like gpss_greedymin() but assume the points marked in ptmk[] are selected already */
static int gpss_greedymin_cont(int *S, int *P, int *ptmk)
{
	int i;
	int S_n = 0;
	for (i = 0; i < pt_n; i++) {
		int p = gpss_greedymin_next(P, ptmk);
		if (p < 0)
			break;
		ptmk[p] = 1;
		S[S_n++] = p;
	}
	return S_n;
}

/* greedily find a subset in general position; add a vertex with the minimum number of collinear points */
static int gpss_greedymin(int *S, int *P)
{
	int *ptmk;	/* indicates if each point is selected */
	int S_n;
	ptmk = calloc(pt_n, sizeof(ptmk[0]));
	S_n = gpss_greedymin_cont(S, P, ptmk);
	free(ptmk);
	return S_n;
}

/* called by gpss_greedymax(): find the next point to remove with the maximum number of collinear points */
static int gpss_greedymax_next(int *P, int *ptmk)
{
	int *lncnt;	/* number of selected points on each line */
	int *ptcnt;	/* number of remaining collinear points for each point */
	int *ptout;	/* indicates if point need not be removed */
	int i, j, k;
	int best = -1;
	lncnt = calloc(ln_n, sizeof(lncnt[0]));
	ptcnt = calloc(pt_n, sizeof(ptcnt[0]));
	ptout = calloc(pt_n, sizeof(ptout[0]));
	for (i = 0; i < pt_n; i++) {
		if (ptmk[i]) {
			for (j = 0; j < pt_cnt[i]; j++)
				lncnt[pt_ln[i][j]]++;
		}
	}
	for (i = 0; i < pt_n; i++) {
		ptout[i] = 1;
		if (!ptmk[i])
			continue;
		for (j = 0; j < pt_cnt[i]; j++)
			if (lncnt[pt_ln[i][j]] > 2)
				ptout[i] = 0;
		for (j = 0; j < pt_cnt[i]; j++) {
			int l = pt_ln[i][j];
			for (k = 0; k < ln_cnt[l]; k++)
				ptcnt[ln[l][k]]++;
		}
	}
	for (i = 0; i < pt_n; i++) {
		int p = P[i];
		if (ptmk[p] && !ptout[p] && (best < 0 || ptcnt[p] > ptcnt[best]))
			best = p;
	}
	free(lncnt);
	free(ptcnt);
	free(ptout);
	return best;
}

/* greedily find a subset in general position; remove a vertex with the maximum number of collinear points */
static int gpss_greedymax(int *S, int *P)
{
	int *ptmk;	/* indicates if each point is removed */
	int S_n = 0;
	int i;
	ptmk = malloc(pt_n * sizeof(ptmk[0]));
	for (i = 0; i < pt_n; i++)
		ptmk[i] = 1;
	for (i = 0; i < pt_n; i++) {
		int p = gpss_greedymax_next(P, ptmk);
		if (p < 0)
			break;
		ptmk[p] = 0;
	}
	for (i = 0; i < pt_n; i++)
		if (ptmk[i])
			S[S_n++] = i;
	free(ptmk);
	return S_n;
}

static void report(int *S, int n)
{
	char mark[NPT] = {0};
	int i;
	for (i = 0; i < n; i++)
		mark[S[i]] = 1;
	printf("%d", pt_n - n);
	for (i = 0; i < pt_n; i++)
		if (!mark[i])
			printf(" %d", i);
	printf("\n");
}

static int gpss_iset(int *filt, int *S, int *P);

static void gpss_part(int *filt, int *P)
{
	int i, j, k;
	for (i = 0; i < pt_n; i++) {
		int p = P[i];
		int n1 = 0;
		int n2 = 0;
		for (j = 0; j < pt_cnt[p]; j++) {	/* decide which part to put p in */
			int l = pt_ln[p][j];
			for (k = 0; k < ln_cnt[l]; k++) {
				if (filt[ln[l][k]] == 1)
					n1++;
				if (filt[ln[l][k]] == 2)
					n2++;
			}
		}
		if (n1 > n2)
			filt[p] = 2;
		if (n2 > n1 || filt[p] == 0)
			filt[p] = 1;
	}
	for (i = 0; i < pt_n; i++)
		filt[P[i]] = filt[P[i]] == 1;
}

static int gpss_main(int *S, int *P)
{
	int *filt;
	int n1, n2, n3;
	int i;
	filt = calloc(pt_n, sizeof(filt[0]));
	gpss_part(filt, P);
	n1 = gpss_iset(filt, S, P);
	for (i = 0; i < pt_n; i++)
		filt[i] = !filt[i];
	n2 = gpss_iset(filt, S + n1, P);
	memset(filt, 0, pt_n * sizeof(filt[0]));
	for (i = 0; i < n1 + n2; i++)
		filt[S[i]] = 1;
	/* adding other points if possible */
	n3 = gpss_greedymin_cont(S + n1 + n2, P, filt);
	free(filt);
	return n1 + n2 + n3;
}

int main(int argc, char *argv[])
{
	int P[NPT];
	int C1[NPT];
	int S1[NPT];
	int S2[NPT];
	int S3[NPT];
	int S4[NPT];
	int c1;
	int n1, n2, n3, n4;
	int rep = 0;		/* report the set of removed points */
	int i;
	for (i = 1; i < argc && argv[i][0] == '-'; i++) {
		if (argv[i][1] == 'r')
			rep = atoi(argv[i][2] ? argv[i] + 2 : argv[++i]);
	}
	data_read(stdin);
	for (i = 0; i < pt_n; i++)
		P[i] = i;
	c1 = gpss_linecover(C1, P);
	n1 = gpss_greedy(S1, P);
	n2 = gpss_greedymin(S2, P);
	n3 = gpss_greedymax(S3, P);
	n4 = gpss_main(S4, P);
	if (rep == 1)
		report(S1, n1);
	if (rep == 2)
		report(S2, n1);
	if (rep == 3)
		report(S3, n1);
	if (rep == 4)
		report(S4, n1);
	if (rep == 5)
		report(C1, c1);
	if (rep == 0)
		printf("%4d %8d %8d %8d %8d\n", pt_n, n4, n1, n2, n3);
	data_free();
	return 0;
}

static int iset_next(int *P, char *mark)
{
	int i, j, k;
	int best = -1;
	int best_odeg = 0;
	for (i = 0; i < pt_n; i++) {
		int p = P[i];
		int odeg = 0;
		if (mark[p])
			continue;
		for (j = 0; j < pt_cnt[p]; j++) {
			int l = pt_ln[p][j];
			for (k = 0; k < ln_cnt[l]; k++)
				if (mark[ln[l][k]] == 0)
					odeg += 1;
		}
		if (best < 0 || odeg < best_odeg) {
			best = p;
			best_odeg = odeg;
		}
	}
	return best;
}

static int gpss_iset(int *filt, int *S, int *P)
{
	/* mark[i]: 0 = not selected; 1 = selected; 2 = filtered or covered */
	char *mark = malloc(pt_n * sizeof(mark[0]));
	int S_n = 0;
	int i, j, k;
	for (i = 0; i < pt_n; i++)
		mark[i] = filt[P[i]] ? 2 : 0;
	for (i = 0; i < pt_n; i++)
		if (!mark[i] && pt_cnt[i] == 0)
			mark[i] = 1;
	for (i = 0; i < pt_n; i++) {
		int p = iset_next(P, mark);
		if (p >= 0) {
			mark[p] = 1;
			for (j = 0; j < pt_cnt[p]; j++) {
				int e = pt_ln[p][j];
				for (k = 0; k < ln_cnt[e]; k++)
					if (!mark[ln[e][k]])
						mark[ln[e][k]] = 2;
			}
		}
	}
	for (i = 0; i < pt_n; i++)
		if (mark[i] == 1)
			S[S_n++] = i;
	free(mark);
	return S_n;
}
