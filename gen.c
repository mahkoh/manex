#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

static inline void xwrite(const char *str)
{
	fwrite(str, strlen(str), 1, stdout);
}

int main(int argc, char **argv)
{
	FILE *input;

	switch (argc) {
	case 1:
		input = stdin;
		break;
	case 2:
		input = fopen(argv[1], "r");
		if (!input)
			die("can't open %s\n", argv[1]);
		break;
	default:
		die("usage: %s [file]\n", argv[0]);
		return 1;
	}

	enum section sec = TEXT;
	char *line = NULL;
	size_t n;
	int len;
	if (getline(&line, &n, input) == -1)
		die("%s is empty\n", argv[1]);
	line[strlen(line)-1] = 0;
	printf(".TH \"%s\"\n", line);
	while ((len = getline(&line, &n, input)) != -1) {
		if (*line == '\n' || *line == ' ') {
			fwrite(line+1, len-1, 1, stdout);
			continue;
		}

		if (sec == CODE)
			xwrite(".fi\n.RE\n");
		if (strcmp(line, CMD_EXAMPLE) == 0) {
			xwrite(".SH EXAMPLE\n");
			sec = TEXT;
		} else if (strcmp(line, CMD_TEXT) == 0) {
			sec = TEXT;
		} else if (strcmp(line, CMD_CODE) == 0) {
			xwrite(".RS\n.nf\n");
			sec = CODE;
		} else {
			die("unknown command: %s", line);
		}
	}
	if (sec == CODE)
		xwrite(".fi\n.RE\n");
}
