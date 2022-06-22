/* Copyright 2021 alloca123

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted, provided that the above copyright notice and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
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
	if(genrss){
	fseek(markdown, 0, SEEK_END);
	long 		fpsize = ftell(markdown);
	fseek(markdown, 0, SEEK_SET);
	char           *file = malloc(fpsize + 1);
	fread(file, 1, fpsize, markdown);
	FILE *fp = fopen("rss.xml", "a+");
	fprintf(fp, "\n\t<item>\n\t\t<title>%s</title>\n\t\t<link>%s%s</link>\n\t\t<description><![CDATA[<pre>\n%s\n</pre>]]></description>\n\t</item>", stuff[1], url, stuff[2] + 1, file);
	free(file);
	fclose (fp);
	}
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
	if(genrss){
	FILE *rss = fopen("rss.xml", "w");
	fprintf(rss, "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n<rss version=\"2.0\" xmlns:atom=\"http://www.w3.org/2005/Atom\">\n<channel>\n<atom:link href=\"%s/rss.xml\" rel=\"self\" type=\"application/rss+xml\"/>\n\t<link>%s</link>\n\t<title>%s</title>\n\t<description>%s</description>\n\t<generator>https://alloca.dev/git/based-ssg</generator>\n", url, url, rsstitle, rssdesc);
	fclose(rss);
	}
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
		if (wc == -1) {
			exit = 1;
		}
	}
	fclose(fp);
	free(file);
	return 0;
}
