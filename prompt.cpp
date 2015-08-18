#include <iostream>
#include <string>
#include <vector>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>

#include "AttributedBlock.hpp"
#include "AttributedString.hpp"

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



AttributedString decorate(const char * const value, const char * const label = NULL)
{
	AttributedString result;
	AttributedBlock block;

	block =  AttributedBlock("[", 219);
	result.append(block);
	if (NULL != label) {
		block = AttributedBlock(label);
		result.append(block);
	}
	block = AttributedBlock(value, 201);
	result.append(block);

	block = AttributedBlock("]", 219);
	result.append(block);

	return result;
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

	std::vector<AttributedString> left;

	std::cout << AttributedBlock("", -1, 225);

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
		std::cout << decorate(shorten_path(envvar, home.c_str()).c_str());
	}

	envvar = getenv("VIRTUAL_ENV");
	if (NULL != envvar) {
		decorate(format_virtualenv(envvar).c_str(), "e|");
	}

	//all_colors(size);

	std::cout << std::endl;

	return 0;
}
