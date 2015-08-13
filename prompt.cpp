#include <iostream>
#include <string>
#include <vector>

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

	size_t length() const
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

class AttributedText {
public:
	void append(AttributedString &s)
	{
		strings.push_back(s);
	}
	size_t length() const
	{
		size_t
			length = 0;

		std::vector<AttributedString>::const_iterator it;
		for (it = strings.cbegin(); it < strings.cend(); ++it) {
			length += it->length();
		}

		return length;
	}

	std::vector<AttributedString> strings;
};
std::ostream& operator<<(std::ostream &os, const AttributedText & text)
{
	std::vector<AttributedString>::const_iterator it;
	for (it = text.strings.cbegin(); it < text.strings.cend(); ++it) {
		os << *it;
	}
	return os;
}

void decorate(const char * const value, const char * const label = NULL)
{
	std::cout << AttributedString("[", 219, 225);
	if (NULL != label) {
		std::cout << label;
	}
	std::cout << AttributedString(value, 201);
	std::cout << AttributedString("]", 219, -1, true) << " ";
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
	printf("\n");

}

std::string shorten_path(const char * const p, const char * const home=NULL)
{
	std::string
		result,
		path(p);
	size_t
		pos = 0,
		next,
		length;
	bool
		shortened = false;

	if (NULL != home) {
		if ((pos = path.find(home)) != std::string::npos) {
			if (0 == pos) {
				path.replace(0, strlen(home), "~");
			}
		}
	}

	pos = 0;

	while (std::string::npos != (next = path.find("/", pos))) {
		next = path.find("/", pos);

		length = next - pos;
		if (4 < length) {
			length = 3;
			shortened = true;
		}
		else {
			shortened = false;
		}

		result.append(path.substr(pos, length));
		if (shortened) {
			result.append("…");
		}

		result.append("/");

		pos = next + 1;
	}
	result.append(path.substr(pos, next - pos));

	return result;
}

std::string format_virtualenv(const char * const venv)
{
	std::string virtual_env(venv);
	size_t pos;

	if (std::string::npos != (pos = virtual_env.rfind("/"))) {
		return virtual_env.substr(pos + 1);
	}
	else {
		return virtual_env;
	}
}

int main(__attribute__((unused)) int argc, __attribute__((unused)) char **argv) {
	TermSize size;

	const char *envvar;
	std::string home;

	/*
	envvar = getenv("USER");
	if (NULL != envvar) {
		decorate(envvar);
	}

	envvar = getenv("SHLVL");
	if (NULL != envvar) {
		decorate(envvar);
	}
	*/

	envvar = getenv("HOME");
	if (NULL != envvar) {
		home = envvar;
	}

	envvar = getenv("PWD");
	if (NULL != envvar) {
		decorate(shorten_path(envvar, home.c_str()).c_str());
	}

	envvar = getenv("VIRTUAL_ENV");
	if (NULL != envvar) {
		decorate(format_virtualenv(envvar).c_str(), "e|");
	}

	//all_colors(size);
	std::cout << std::endl;

	return 0;
}
