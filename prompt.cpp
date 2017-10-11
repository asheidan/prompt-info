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
	unsigned char path;
} ColorScheme;

const ColorScheme InsertScheme = {
	.background = 64,
	.bracket = 70,
	.separator = 70,
	.label = 70,
	.information = 76,
	.suffix = 76,
	.host = 82,
	.path = 255
};

const ColorScheme NormalScheme = {
	.background = 25,
	.bracket = 31,
	.separator = 31,
	.label = 31,
	.information = 37,
	.suffix = 37,
	.host = 43,
	.path = 255
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

		if (0 >= cols) {
			cols = 80;
		}

		if (0 >= rows) {
			rows = 24;
		}
	}

	int cols;
	int rows;
};


AttributedString decorate_path(const char * const value)
{
	AttributedString result;
	AttributedBlock block;

	//block = AttributedBlock("[", CurrentScheme->bracket);
	//result.append(block);
	block = AttributedBlock(value, 12);
	result.append(block);

	//block = AttributedBlock("]", CurrentScheme->bracket);
	//result.append(block);

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

AttributedString decorate(const char * const value, int color)
{
	AttributedString result;
	AttributedBlock block;

	block = AttributedBlock(value, color);
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

std::string format_docker_host(const char * const url)
{
	std::string docker_host(url);
	size_t pos;

	docker_host.replace(docker_host.begin(), docker_host.begin() + 6, "");

	if (std::string::npos != (pos = docker_host.rfind(":"))) {
		docker_host.replace(docker_host.begin() + pos, docker_host.end(), "");
	}

	return docker_host;
}

int main(int argc, char **argv) {
	//TermSize size;

	const char *envvar;

	std::string home;

	std::vector<AttributedString>
		left;

	/*
	if (1 < argc) {
		if ('n' == argv[1][0]) {
			CurrentScheme = &NormalScheme;
		}
		else if ('c' == argv[1][0]) {
			CurrentScheme = &NormalScheme;
		}
		else if ('i' == argv[1][0]) {
			CurrentScheme = &InsertScheme;
		}
	}
	*/

	// TODO: Check the variable SUDO_USER
	envvar = getenv("USER");
	if (NULL != envvar) {
		const char *logname;
		logname = getenv("LOGNAME");
		if (NULL != logname) {
			if (0 != strcmp(envvar, logname)) {
				left.push_back(decorate(envvar, 9));
				left.push_back(decorate("in", 8));
			}
		}
		const char *sudo_user;
		sudo_user = getenv("SUDO_USER");
		if (NULL != sudo_user) {
			if (0 != strcmp(envvar, sudo_user)) {
				left.push_back(decorate(envvar, 9));
				left.push_back(decorate("in", 8));
			}
		}
	}

	envvar = getenv("HOME");
	if (NULL != envvar) {
		home = envvar;
	}

	envvar = getenv("PWD");
	if (NULL != envvar) {
		left.push_back(decorate(shorten_path(envvar, home.c_str()).c_str(), 12));
	}

#ifdef LIBGIT2_AVAILABLE
	try {
		GitRepo repo(envvar);

		std::string branchname = repo.branch();
		if ("feature/" == branchname.substr(0, 8)) {
			branchname.replace(0, 8, "f~/");
		}
		if ("hotfix/" == branchname.substr(0, 7)) {
			branchname.replace(0, 7, "h~/");
		}

		left.push_back(decorate("on", 8));
		left.push_back(decorate(branchname.c_str(), 13));
		std::string status = repo.status();
		if (0 != status.compare("  ")) {
			status.insert(0, "[");
			status.append("]");
			left.push_back(decorate(status.c_str(), 9));
		}
	}
	catch(std::exception& e) {
		//std::cout << e.what() << std::endl;
	}
#endif

	/*
	envvar = getenv("VIRTUAL_ENV");
	if (NULL != envvar) {
		left.push_back(decorate(format_virtualenv(envvar).c_str(), "e"));
	}

	envvar = getenv("JAVA_HOME");
	if (NULL != envvar) {
		left.push_back(decorate(format_java_home(envvar).c_str(), "j"));
	}

	envvar = getenv("DOCKER_HOST");
	if (NULL != envvar) {
		left.push_back(decorate(format_docker_host(envvar).c_str(), "d"));
	}
	*/

	envvar = getenv("SSH_CONNECTION");
	if (NULL != envvar && '\0' != *envvar) {
		left.push_back(decorate("@", 8));
		left.push_back(decorate(hostname().c_str(), 8));
	}

	//all_colors(size);

	std::cout << left;

	// Bash uses %L, Zsh uses %E to erase to end of line

	std::cout << "\x1B[K\x1B[0m" << std::endl;

	return 0;
}
