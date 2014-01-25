#include <dirent.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <unistd.h>

#define max(x,y) (((x)>(y))?(x):(y))

int argc;
char **argv;
char *path;

static void die(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	exit(1);
}

static int xstrcmp(const void *a, const void *b)
{
	char * const *as = a;
	char * const *bs = b;
	return strcmp(*as, *bs);
}

static int term_width(void)
{
	struct winsize w;
	ioctl(1, TIOCGWINSZ, &w);
	return w.ws_col;
}

static void print_cols(char **buf, int num)
{
	int tw = term_width();
	int max_width = 0;
	int *slens = malloc(num * sizeof(*slens));
	for (int i = 0; i < num; i++) {
		slens[i] = strlen(buf[i]);
		max_width = max(max_width, slens[i]);
	}
	max_width += 2;
	int cols = tw / max_width;
	cols = cols ? cols : 1;
	int rows = (num+cols-1)/cols;
	int rest = num%rows;
	cols = num/rows + !!rest;
	for (int i = 0; i < rows; i++) {
		if (i == rest)
			cols -= !!rest;
		for (int j = 0; j < cols; j++) {
			int k = j*rows + i;
			printf("%s", buf[k]);
			int spaces = max_width - slens[k];
			while (spaces--)
				printf(" ");
		}
		printf("\n");
	}
}

static int get_next(char ***dest)
{
	DIR *dir = opendir(path);
	struct dirent *ent;
	int len = 0;
	int cap = 20;
	char **buf = malloc(cap * sizeof(*buf));
	while ((ent = readdir(dir))) {
		if (ent->d_name[0] == '.')
			continue;
		if (len + 1 == cap) {
			cap *= 2;
			buf = realloc(buf, cap * sizeof(*buf));
		}
		buf[len++] = strdup(ent->d_name);
	}
	qsort(buf, len, sizeof(*buf), xstrcmp);
	*dest = buf;
	return len;
}

static void print_next(void)
{
	char **next;
	int num = get_next(&next);
	if (!isatty(1)) {
		for (int i = 0; i < num; i++)
			printf("%s\n", next[i]);
	} else if (num) {
			print_cols(next, num);
	}
}

static void check_path(void)
{
	struct stat st;
	if (stat(path, &st)) {
		printf("Can't access ");
		for (int i = 0; i < argc; i++)
			printf("%s ", argv[i]);
		printf("\n");
		exit(1);
	}
	if ((st.st_mode & S_IFDIR) == 0)
		return;
	print_next();
	exit(1);
}

static void make_path(void)
{
	size_t len = strlen(DOCDIR) + 1;
	for (int i = 0; i < argc; i++)
		len += strlen(argv[i]) + 1;
	path = malloc(len);
	char *cur = stpcpy(path, DOCDIR);
	for (int i = 0; i < argc; i++) {
		*cur++ = '/';
		cur = stpcpy(cur, argv[i]);
	}
}

int main(int ac, char **av)
{
	argc = ac - 1;
	argv = av + 1;
	make_path();
	check_path();
	execlp("man", "man", path, (char *)NULL);
}
