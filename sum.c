#include <stdio.h>

#define NALG		4
#define MAXR		(1 << 12)

int main(void)
{
	int cnt = 0;
	int npt;
	int r[NALG];
	int amax[NALG] = {0};
	int arat[NALG] = {0};
	int i;
	while (scanf("%d", &npt) == 1) {
		int best = 0;
		for (i = 0; i < NALG; i++)
			scanf("%d", &r[i]);
		for (i = 0; i < NALG; i++)
			if (best < r[i])
				best = r[i];
		for (i = 0; i < NALG; i++)
			if (r[i] == best)
				amax[i]++;
		for (i = 0; i < NALG; i++)
			arat[i] += r[i] * 10000 / best;
		cnt++;
	}
	for (i = 0; i < NALG; i++) {
		int maxrat = amax[i] * 10000 / cnt;
		int sizerat = arat[i] / cnt;
		printf("%2d.%02d %2d.%02d\n", maxrat / 100, maxrat % 100,
			sizerat / 100, sizerat % 100);
	}
	return 0;
}
