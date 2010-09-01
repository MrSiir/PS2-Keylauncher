/* rawtoasm */
/* SoopaDoopa 2001 */

#include <stdio.h>

int main(int argc, char **argv)
{
	static char in[1024];
	static char out[1024];
	static char labels[1024];
	static char labele[1024];
	FILE *f;
	int b;
	char h=' ';

	if(argc!=4)
	{
		printf("rawtoasm.exe infile outfile label\n");

		exit(0);
	}

	{
		char *p=argv[3];
		while(*p)
		{
			if(*p=='.')*p=0;
			else p++;
		}
	}

	sprintf(in,"%s",argv[1]);
	sprintf(out,"%s",argv[2]);
	sprintf(labels,"binary_%s_start",argv[3]);
	sprintf(labele,"binary_%s_end",argv[3]);

	freopen(out,"w",stdout);

	f=fopen(in,"rb");

	printf(".set noreorder\n.globl  %s\n.text\n.align	7\n%s:	.byte",labels,labels);
	while((b=fgetc(f))!=EOF)
	{
		printf("%c%i",h,b);
		h=',';
	}
	printf("\n");
	printf(".globl  %s\n%s:\n",labele,labele);

	fclose(f);
	fclose(stdout);
}

