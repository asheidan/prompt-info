#include <iostream>
#include <string>
#include <cstring>
#include <vector>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/stat.h>

#ifdef LIBGIT2_AVAILABLE
#include "git_status.hpp"
#endif

#include "AttributedBlock.hpp"
#include "AttributedString.hpp"


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

std::string find_walk_upwards(std::string &path, const char *filename) {
	std::string
		current_path = path,
		file_path;
	struct stat
		stat_buffer;
	size_t
		pos;

	for (pos = current_path.length(); std::string::npos != pos; pos = current_path.rfind("/")) {
		current_path = current_path.substr(0, pos);
		file_path = current_path;
		file_path.append("/");
		file_path.append(filename);
		if (0 == stat(file_path.c_str(), &stat_buffer)) {
			return file_path;
		}
	}
	return std::string("");
}


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
		java_home.replace(java_home.begin(), java_home.begin() += pos + 1, "");
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

int main(int argc __attribute__((unused)), char **argv __attribute__((unused))) {
	//TermSize size;

	const char *envvar;

	std::string home;
	std::string path;

	std::vector<AttributedString>
		environment, tools;

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

	// Environment variables that affect the behaviour of tools

	envvar = getenv("AWS_PROFILE");
	std::string aws_profile;
	if (envvar) {
		aws_profile = std::string(envvar);
	}
	if (aws_profile.length()) {
		AttributedString aws_info;
		AttributedBlock block;

		block = AttributedBlock("aws:", 3);
		aws_info.append(block);

		if (aws_profile.length()) {
			block = AttributedBlock(aws_profile.c_str(), 9);
			aws_info.append(block);
		}

		// Make sure load config is enabled
		envvar = getenv("AWS_SDK_LOAD_CONFIG");
		if (!envvar || strcmp(envvar, "1")) {
			block = AttributedBlock("?!", 1);
			aws_info.append(block);
		}

		environment.push_back(aws_info);

	}
	envvar = getenv("KUBECTL_CONTEXT");
	std::string kube_context;
	if (envvar) {
		kube_context = std::string(envvar);
	}
	envvar = getenv("KUBECTL_NAMESPACE");
	std::string kube_namespace;
	if (envvar) {
		kube_namespace = std::string(envvar);
	}
	if (kube_context.length() or kube_namespace.length()) {
		AttributedString k8s_info;
		AttributedBlock block;

		block = AttributedBlock("k8s:", 6);
		k8s_info.append(block);

		if (kube_context.length()) {
			block = AttributedBlock(kube_context.c_str(), 14);
			k8s_info.append(block);
		}
		if (kube_namespace.length()) {
			block = AttributedBlock("[", 6);
			k8s_info.append(block);

			block = AttributedBlock(kube_namespace.c_str(), 14);
			k8s_info.append(block);

			block = AttributedBlock("]", 6);
			k8s_info.append(block);
		}
		environment.push_back(k8s_info);
	}

	envvar = getenv("DOCKER_CERT_PATH");
	if (NULL != envvar) {
		std::string cert_path(envvar);
		if (std::string::npos != cert_path.find(".minikube")) {
			// We have probably set docker to use minikube
			AttributedString docker_info;
			AttributedBlock block;

			block = AttributedBlock("docker:", 4);
			docker_info.append(block);

			block = AttributedBlock("minikube", 12);
			docker_info.append(block);

			environment.push_back(docker_info);
		}
	}

	// TODO: Add check for http(s)_proxy
	envvar = getenv("https_proxy");
	std::string https_proxy;
	if (NULL != envvar) {
		https_proxy = std::string(envvar);
	}
	envvar = getenv("http_proxy");
	std::string http_proxy;
	if (NULL != envvar) {
		http_proxy = std::string(envvar);
	}
	if (http_proxy.length() or https_proxy.length()) {
		if (http_proxy == https_proxy) {
			AttributedString proxy_info;
			AttributedBlock block;

			block = AttributedBlock("http/https:", 3);
			proxy_info.append(block);

			block = AttributedBlock(http_proxy, 11);
			proxy_info.append(block);

			environment.push_back(proxy_info);
		}
		else {
			if (http_proxy.length()) {
				AttributedString proxy_info;
				AttributedBlock block;

				block = AttributedBlock("http:", 3);
				proxy_info.append(block);

				block = AttributedBlock(http_proxy, 11);
				proxy_info.append(block);

				environment.push_back(proxy_info);
			}
			if (https_proxy.length()) {
				AttributedString proxy_info;
				AttributedBlock block;

				block = AttributedBlock("https:", 3);
				proxy_info.append(block);

				block = AttributedBlock(https_proxy, 11);
				proxy_info.append(block);

				environment.push_back(proxy_info);
			}
		}
	}

	if (environment.size() > 0) {
		std::cout << environment << std::endl;
	}

	// General settings
	envvar = getenv("USER");
	if (NULL != envvar) {
		if (0 == strcmp(envvar, "root")) {
			std::cout << decorate(envvar, 9) << " " << decorate("in", 8) << " ";
		}
		else {
			const char *logname;
			logname = getenv("LOGNAME");
			if (NULL != logname) {
				if (0 != strcmp(envvar, logname)) {
					std::cout << decorate(envvar, 9) << " " << decorate("in", 8) << " ";
				}
			}
		}
	}

	envvar = getenv("HOME");
	if (NULL != envvar) {
		home = envvar;
	}

	envvar = getenv("PWD");
	if (NULL != envvar) {
		std::cout << decorate(shorten_path(envvar, home.c_str()).c_str(), 12);
		path = envvar;
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

		std::cout << " " << decorate("on", 8);
		std::cout << " " << decorate(branchname.c_str(), 13);
		std::string status = repo.status();
		if (0 != status.compare("  ")) {
			status.insert(0, "[");
			status.append("]");
			std::cout << " " << decorate(status.c_str(), 9);
		}
	}
	catch(std::exception& e) {
		//std::cout << e.what() << std::endl;
	}
#endif

	envvar = getenv("VIRTUAL_ENV");
	if (NULL != envvar) {
		AttributedString venv_info;
		AttributedBlock block;

		block = AttributedBlock("venv:", 2);
		venv_info.append(block);

		block = AttributedBlock(format_virtualenv(envvar).c_str(), 10);
		venv_info.append(block);

		tools.push_back(venv_info);
	}

	envvar = getenv("JAVA_HOME");
	if (NULL != envvar) {
		AttributedString java_info;
		AttributedBlock block;

		block = AttributedBlock("java:", 2);
		java_info.append(block);

		block = AttributedBlock(format_java_home(envvar).c_str(), 10);
		java_info.append(block);

		tools.push_back(java_info);
	}

	std::string cargo_path = find_walk_upwards(path, "Cargo.toml");
	if (cargo_path.length()) {
		AttributedString rust_info;
		AttributedBlock block;

		block = AttributedBlock("rust:", 2);
		rust_info.append(block);

		tools.push_back(rust_info);
	}

	std::string make_path = find_walk_upwards(path, "Makefile");
	if (make_path.length()) {
		AttributedString make_info;
		AttributedBlock block;

		block = AttributedBlock("make", 4);
		make_info.append(block);

		tools.push_back(make_info);
	}

	std::string gradle_path = find_walk_upwards(path, "build.gradle");
	if (gradle_path.length()) {
		AttributedString gradle_info;
		AttributedBlock block;

		block = AttributedBlock("gradle", 4);
		gradle_info.append(block);

		tools.push_back(gradle_info);
	}

	std::string scons_path = find_walk_upwards(path, "SConstruct");
	if (scons_path.length()) {
		AttributedString scons_info;
		AttributedBlock block;

		block = AttributedBlock("scons", 4);
		scons_info.append(block);

		tools.push_back(scons_info);
	}

	std::string vagrant_path = find_walk_upwards(path, "Vagrantfile");
	if (vagrant_path.length()) {
		AttributedString vagrant_info;
		AttributedBlock block;

		block = AttributedBlock("[v]", 12);
		vagrant_info.append(block);

		tools.push_back(vagrant_info);
	}

	if (tools.size() > 0) {
		AttributedBlock separator(",", 8);

		std::cout << " " << decorate("using", 8) << " ";
		std::vector<AttributedString>::const_iterator it = tools.begin();
		std::cout << *it;
		for (++it; it < tools.end(); ++it) {
			std::cout << separator << *it;
		}
	}

	envvar = getenv("SSH_CONNECTION");
	if (NULL != envvar && '\0' != *envvar) {
		std::cout << " " << decorate("@", 8) << " " << decorate(hostname().c_str(), 8);
	}

	//all_colors(size);
	envvar = getenv("SHLVL");
	if (NULL != envvar && strtol(envvar, NULL, 10) > 1) {
		std::cout << decorate(" lvl ", 15) << decorate(envvar, 15);
	}


	// Bash uses %L, Zsh uses %E to erase to end of line

	std::cout << "\x1B[K\x1B[0m" << std::endl;

	return 0;
}
