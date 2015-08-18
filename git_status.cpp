#include <iostream>

#include <git2/repository.h>

int main(__attribute__((unused)) int argc, __attribute__((unused)) char **argv)
{
	int error;
	git_repository *repo;

	error = git_repository_open_ext(&repo, "/Users/emieri/Documents/Projects/prompt-info", 0, NULL);
	if (0 > error) {

	}

	return 0;
}
