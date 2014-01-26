#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
#include <ftw.h>

#define CMD_EXAMPLE "example\n"
#define CMD_TEXT    "text\n"
#define CMD_CODE    "code\n"

enum section {
	TEXT,
	CODE,
};

static void die(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	exit(1);
}

static void xwrite(FILE *out, const char *str)
{
	fwrite(str, strlen(str), 1, out);
}

static void generate(FILE *in, FILE *out)
{
	enum section sec = TEXT;
	char *line = NULL;
	size_t n;
	int len;
	if (getline(&line, &n, in) == -1)
		die("source is empty\n");
	line[strlen(line)-1] = 0;
	fprintf(out, ".TH \"%s\"\n", line);
	while ((len = getline(&line, &n, in)) != -1) {
		if (*line == '\n' || *line == ' ') {
			fwrite(line+1, len-1, 1, out);
			continue;
		}

		if (sec == CODE)
			xwrite(out, ".fi\n.RE\n");
		if (strcmp(line, CMD_EXAMPLE) == 0) {
			xwrite(out, ".SH EXAMPLE\n");
			sec = TEXT;
		} else if (strcmp(line, CMD_TEXT) == 0) {
			sec = TEXT;
		} else if (strcmp(line, CMD_CODE) == 0) {
			xwrite(out, ".RS\n.nf\n");
			sec = CODE;
		} else {
			die("unknown command: %s", line);
		}
	}
	if (sec == CODE)
		xwrite(out, ".fi\n.RE\n");
}

static void mkdirp(char *doc)
{
	for (char *cur = doc; *cur; cur++) {
		if (*cur == '/') {
			*cur = 0;
			mkdir(doc, S_IRWXU | S_IXGRP | S_IXOTH);
			*cur = '/';
		}
	}
}

static int is_newer(const char *doc, time_t pre)
{
	struct stat doc_stat;
	if (stat(doc, &doc_stat) == 0) {
		if (pre <= doc_stat.st_mtime)
			return 0;
		else
			return 1;
	} else if (errno != ENOENT) {
		die("Can't access %s (%s)\n", doc, strerror(errno));
	}
	return 1;
}

static int get_doc(const char *path, char **doc)
{
	char *slash = strchr(path, '/');
	if (slash == NULL)
		return 0;
	char *ext = strrchr(slash, '.');
	if (ext == NULL || ext[1] != 'm' || ext[2] != 'x')
		return 0;
	char *dest = malloc(sizeof("docs")+ext-slash);
	char *cur = mempcpy(dest, "docs", strlen("docs"));
	cur = mempcpy(cur, slash, ext-slash);
	*cur = 0;
	*doc = dest;
	return cur-dest;
}

static int walker(const char *path, const struct stat *mx_stat, int type)
{
	char *doc = NULL;
	int doc_len;

	if ( type != FTW_F
	  || (doc_len = get_doc(path, &doc)) == 0
	  || !is_newer(doc, mx_stat->st_mtime))
		goto OUT;
	xwrite(stdout, "GEN ");
	fwrite(doc, doc_len, 1, stdout);
	xwrite(stdout, "\n");
	mkdirp(doc);
	FILE *in = fopen(path, "r");
	FILE *out = fopen(doc, "w");
	generate(in, out);
	fclose(out);
	fclose(in);
OUT:
	free(doc);
	return 0;
}

int main(int argc, char **argv)
{
	ftw("pre", walker, 16);
}
