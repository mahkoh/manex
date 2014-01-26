#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <ftw.h>

time_t bin_time;

enum section {
	ERROR,
	EXAMPLE,
	TEXT,
	CODE,
};

static void xwrite(FILE *out, const char *str)
{
	fwrite(str, strlen(str), 1, out);
}

static int xstrcmp(const char *a, const char *b)
{
	return !strncmp(a, b, strlen(b));
}

static enum section get_section(const char *line)
{
	if (xstrcmp(line, "example"))
		return EXAMPLE;
	if (xstrcmp(line, "text"))
		return TEXT;
	if (xstrcmp(line, "code"))
		return CODE;
	return ERROR;
}

static void generate(const char *in_path, const char *out_path)
{
	const char *error = NULL;
	FILE *in = fopen(in_path, "r");
	FILE *out = fopen(out_path, "w");

	enum section sec = TEXT;
	char *line = NULL;
	size_t n;
	int len;
	if (getline(&line, &n, in) == -1) {
		error = "source is empty\n";
		goto ERROR;
	}
	line[strlen(line)-1] = 0;
	fprintf(out, ".TH \"%s\"\n", line);
	while ((len = getline(&line, &n, in)) != -1) {
		if (*line == '\n' || *line == ' ') {
			fwrite(line+1, len-1, 1, out);
			continue;
		}

		if (sec == CODE)
			xwrite(out, ".fi\n.RE\n");
		switch ((sec = get_section(line))) {
		case EXAMPLE:
			xwrite(out, ".SH EXAMPLE\n");
			break;
		case TEXT:
			break;
		case CODE:
			xwrite(out, ".RS\n.nf\n");
			break;
		default:
			error = "unknown command: %s";
			goto ERROR;
		}
	}
	if (sec == CODE)
		xwrite(out, ".fi\n.RE\n");

ERROR:
	fclose(in);
	fclose(out);
	if (error) {
		fprintf(stderr, error, line);
		unlink(out_path);
		exit(1);
	}
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
		return bin_time > doc_stat.st_mtime || pre > doc_stat.st_mtime;
	} else if (errno != ENOENT) {
		fprintf(stderr, "Can't access %s (%s)\n", doc, strerror(errno));
		exit(1);
	}
	return 1;
}

static char *get_doc(const char *path)
{
	char *slash = strchr(path, '/');
	if (slash == NULL)
		return NULL;
	char *ext = strrchr(slash, '.');
	if (ext == NULL || ext[1] != 'm' || ext[2] != 'x')
		return NULL;
	char *dest = malloc(sizeof("docs")+ext-slash);
	char *cur = mempcpy(dest, "docs", strlen("docs"));
	cur = mempcpy(cur, slash, ext-slash);
	*cur = 0;
	return dest;
}

static int walker(const char *path, const struct stat *mx_stat, int type)
{
	char *doc = NULL;

	if ( type == FTW_F
	  && (doc = get_doc(path)) != NULL
	  && is_newer(doc, mx_stat->st_mtime)) {
		printf("GEN %s\n", doc);
		mkdirp(doc);
		generate(path, doc);
	}
	free(doc);
	return 0;
}

int main(int argc, char **argv)
{
	struct stat bin_stat = { 0 };
	stat(argv[0], &bin_stat);
	bin_time = bin_stat.st_mtime;
	ftw("pre", walker, 16);
}
