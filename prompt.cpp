#include <iostream>
#include <string>
#include <cstring>
#include <vector>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>

#ifdef LIBGIT2_AVAILABLE
#include "git_status.hpp"
#endif

#include "AttributedBlock.hpp"
#include "AttributedString.hpp"


typedef struct {
	unsigned char background;
	unsigned char bracket;
	unsigned char separator;
	unsigned char label;
	unsigned char information;
	unsigned char suffix;
	unsigned char host;
} ColorScheme;

const ColorScheme InsertScheme = {
	.background = 64,
	.bracket = 70,
	.separator = 70,
	.label = 70,
	.information = 76,
	.suffix = 70,
	.host = 82
};

const ColorScheme NormalScheme = {
	.background = 25,
	.bracket = 31,
	.separator = 31,
	.label = 31,
	.information = 37,
	.suffix = 31,
	.host = 43
};

const ColorScheme *CurrentScheme = &InsertScheme;

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
		struct winsize ts;
		ioctl(STDIN_FILENO, TIOCGWINSZ, &ts);
		cols = ts.ws_col;
		rows = ts.ws_row;
#endif
	}

	int cols;
	int rows;
};


AttributedString decorate_path(const char * const value)
{
	AttributedString result;
	AttributedBlock block;

	block = AttributedBlock("[", 70);
	result.append(block);
	block = AttributedBlock(value, 255);
	result.append(block);

	block = AttributedBlock("]", 70);
	result.append(block);

	return result;
}

AttributedString decorate_user_host(const char * const user, const char * const host)
{
	AttributedString result;
	AttributedBlock block;

	block = AttributedBlock("[", CurrentScheme->bracket);
	result.append(block);

	block = AttributedBlock(user, CurrentScheme->information);
	result.append(block);

	block = AttributedBlock("@", CurrentScheme->separator);
	result.append(block);

	block = AttributedBlock(host, CurrentScheme->host);
	result.append(block);

	block = AttributedBlock("]", CurrentScheme->bracket);
	result.append(block);

	return result;
}

AttributedString decorate(const char * const value,
						  const char * const label = NULL,
						  const char * const suffix = NULL)
{
	AttributedString result;
	AttributedBlock block;

	block = AttributedBlock("[", CurrentScheme->bracket);
	result.append(block);
	if (NULL != label) {
		block = AttributedBlock(label, CurrentScheme->label);
		result.append(block);
		block = AttributedBlock("|", CurrentScheme->separator);
		result.append(block);
	}
	block = AttributedBlock(value, CurrentScheme->information);
	result.append(block);

	if (NULL != suffix) {
		block = AttributedBlock("|", CurrentScheme->separator);
		result.append(block);

		block = AttributedBlock(suffix, CurrentScheme->suffix);
		result.append(block);
	}

	block = AttributedBlock("]", CurrentScheme->bracket);
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
			result.append("~");
		}

		result.append("/");

		pos = next + 1;
	}
	result.append(path.substr(pos, next - pos));

	return result;
}

std::string hostname()
{
	char hostname[256];
	char *p = hostname;

	std::string result;

	gethostname(hostname, 256);

	for (; *p != '.' && *p != '\0' && p < (hostname + 256); ++p) {}
	*p = '\0';

	result = hostname;

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

std::string format_java_home(const char * const java_var)
{
	std::string java_home(java_var);
	size_t pos;

	if (std::string::npos != (pos = java_home.rfind(".jdk/Contents/Home"))) {
		java_home.replace(java_home.begin() += pos, java_home.end(), "");
	}

	if (std::string::npos != (pos = java_home.rfind("/"))) {
		java_home.replace(java_home.begin(), java_home.begin() += pos + 4, "");
	}

	return java_home;
}

int main(int argc, char **argv) {
	TermSize size;

	const char *envvar;

	std::string home;

	std::vector<AttributedString>
		left, right;

	if (1 < argc) {
		if ('n' == argv[1][0]) {
			CurrentScheme = &NormalScheme;
		}
		else if ('i' == argv[1][0]) {
			CurrentScheme = &InsertScheme;
		}
	}

	envvar = getenv("USER");
	if (NULL != envvar) {
		right.push_back(decorate_user_host(envvar, hostname().c_str()));
	}

	/*
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
		left.push_back(decorate_path(shorten_path(envvar, home.c_str()).c_str()));
	}

#ifdef LIBGIT2_AVAILABLE
	try {
		GitRepo repo(envvar);

		left.push_back(decorate(repo.branch().c_str(), "g", repo.status().c_str()));
	}
	catch(std::exception& e) {
		//std::cout << e.what() << std::endl;
	}
#endif

	envvar = getenv("VIRTUAL_ENV");
	if (NULL != envvar) {
		left.push_back(decorate(format_virtualenv(envvar).c_str(), "e"));
	}

	envvar = getenv("JAVA_HOME");
	if (NULL != envvar) {
		left.push_back(decorate(format_java_home(envvar).c_str(), "j"));
	}


	//all_colors(size);

	//std::cout << "cols: " << size.cols << std::endl;
	//std::cout << "left: " << length(left) << std::endl;
	//std::cout << "right: " << length(right) << std::endl;

	std::cout << AttributedBlock("", -1, CurrentScheme->background);

	std::cout << left;

	for (size_t i = size.cols - length(left) - length(right); i > 0; --i) {
		std::cout << " "; // i % 10;
	}

	std::cout << right;
	std::cout << "\x1B[0m" << std::endl;

	return 0;
}
