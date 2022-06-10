#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#if defined(__Openbsd__)
#include <unistd.h>
#endif
#include "config.h"
int 		smu_convert(FILE * out, FILE * source, int supresshtml);
/* function to find and replac the title */
char           *
replaceword(const char *s, const char *oldW,
	    const char *newW)
{
	char           *result;
	int 		i        , cnt = 0;
	int 		newWlen = strlen(newW);
	int 		oldWlen = strlen(oldW);
	for (i = 0; s[i] != '\0'; i++) {
		if (strstr(&s[i], oldW) == &s[i]) {
			cnt++;
			i += oldWlen - 1;
		}
	}
	result = (char *) malloc(i + cnt * (newWlen - oldWlen) + 1);
	i = 0;
	while (*s) {
		if (strstr(s, oldW) == s) {
			strcpy(&result[i], newW);
			i += newWlen;
			s += oldWlen;
		} else
			result[i++] = *s++;
	}

	result[i] = '\0';
	return result;
}

void
generate_files(char *line)
{
	/* declare and open the template file */
	FILE           *template;
	template = fopen(htmltemplate, "r");
	fseek(template, 0, SEEK_END);
	/* get the template size */
	long 		templatesize = ftell(template);
	fseek(template, 0, SEEK_SET);
	/* allocate the memory for reading the template file */
	char           *file = malloc(templatesize + 1);
	/* read the template file */
	fread(file, 1, templatesize, template);
	fclose(template);

	char           *token, *stuff[3];
	int 		i = 0;
	token = strtok(line, "^");
	while (token != NULL) {
		stuff[i] = token;
		token = strtok(NULL, "^");
		i++;
	}
	/* find "INSERT-TITLE" and replace it with the specified title */
	char           *result = replaceword(file, "INSERT-TITLE", stuff[1]);
	free(file);
	FILE           *article;
	article = fopen(stuff[2], "w+");
	fprintf(article, "%s", result);
	free(result);
	fclose(article);

	FILE           *article2, *markdown;
	article2 = fopen(stuff[2], "a+");
	markdown = fopen(stuff[0], "rw+");
	smu_convert(article2, markdown, 0);
	rewind(article2);
	fputs(appendhtml, article2);
	fclose(article2);
	fclose(markdown);
}

int
main(int argc, char *argv[])
{
#if defined(__Openbsd__)
	pledge("stdio rpath wpath", NULL);
#endif
	if (argc < 2) {
		printf("usage:\nbased-ssg files.conf\n");
		return 0;
	}
	FILE           *fp;
	/* open file */
	fp = fopen(argv[1], "r");
	/* nc is the number of lines in the file */
	int 		nc       , exit, wc, y;
	exit = nc = 0;
	while ((y = fgetc(fp)) != EOF) {
		if (y == '\n')
			nc++;
	}
	rewind(fp);
	char 		c        [200];
	wc = nc - 1;
	fseek(fp, 0, SEEK_END);
	long 		fpsize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char           *file = malloc(fpsize + 1);
	fread(file, 1, fpsize, fp);
	char *lines[nc], *token;
	int i = 0;
	token = strtok(file, "\n");
	while (token != NULL) {
		lines[i] = token;
		token = strtok(NULL, "\n");
		i++;
	}
	while (!exit) {
		/* build the pages */
		generate_files(lines[wc]);
		wc--;
		if (wc -1) {
			exit = 1;
		}
	}
	fclose(fp);
	free(file);
	return 0;
}
