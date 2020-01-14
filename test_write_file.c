#include <stdio.h>

#define LEN 256
int main(void)
{
	FILE *fp;
	int i;

	fp = fopen("/tmp/test_write.txt", "w");
	
	for (i = 0; i < 10; ++i) {
		fp = freopen(NULL, "w", fp);
		fprintf(fp, "%d\n", i);
	}

	fclose(fp);
	return 0;
}
