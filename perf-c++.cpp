#include <iostream>
#include <string>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>

// green: \x1B[32m
/**
 * 256color:
 * FG: \x1B[38;5;%dm
 * BG: \x1B[48;5;%dm
 */

class TermSize {
public:
	TermSize() : cols(80), rows(24)
	{
#ifdef TIOCGSIZE
		struct ttysize ts;
		ioctl(STDIN_FILENO, TIOCGSIZE, &ts);
		cols = ts.ts_cols;
		rows = ts.ts_lines;
#elif defined(TIOCGWINSZ)
		struct winsize &ts;
		ioctl(STDIN_FILENO, TIOCGWINSZ, &ts);
		cols = ts.ws_col;
		rows = ts.ws_row;
		#endif
	}

	int cols;
	int rows;
};

typedef int color_t;
class AttributedString {
public:
	AttributedString(const std::string &content,
					 const color_t foreground = -1, const color_t background = -1,
					 bool reset=false)
		: content(content), foreground(foreground), background(background), reset(reset)
	{}

	size_t length()
	{
		return content.length();
	}

	std::string content;
	color_t foreground;
	color_t background;
	bool reset;
};
std::ostream& operator<<(std::ostream &os, const AttributedString & str)
{
	if (0x0 <= str.foreground && str.foreground <= 0xFF) {
		os << "\x1B[38;5;" << str.foreground << "m";
	}
	if (0x0 <= str.background && str.background <= 0xFF) {
		os << "\x1B[48;5;" << str.background << "m";
	}

	os << str.content;

	if (str.reset) {
		os << "\x1B[0m";
	}

	return os;
}

void decorate(const char * const str)
{
	std::cout << AttributedString("[", 219);
	std::cout << AttributedString(str, 201);
	std::cout << AttributedString("]", 219, -1, true) << std::endl;
}

void all_colors(TermSize &size)
{
	int fieldcount = size.cols / 4;
	for (int i = 0; i < 0xFF; ++i) {
		printf("\x1B[38;5;%dm%03d\x1B[0m ", i, i);
		if ((fieldcount - 1) == (i % fieldcount)) {
			printf("\n");
		}
	}

}

int main(__attribute__((unused)) int argc, __attribute__((unused)) char **argv) {
	TermSize size;

	const char *envvar;

	envvar = getenv("SHLVL");
	if (NULL != envvar) {
		decorate(envvar);
	}

	all_colors(size);

	return 0;
}
