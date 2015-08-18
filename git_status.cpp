#include <iostream>
#include <stdio.h>

#include <git2/diff.h>
#include <git2/errors.h>
#include <git2/global.h>
#include <git2/repository.h>
#include <git2/strarray.h>
#include <git2/status.h>


int print_git_error(int error)
{
	const git_error *e = giterr_last();
	fprintf(stderr, "Error: %d/%d: %s\n", error, e->klass, e->message);

	return error;
}

void show_branch(git_repository * const repo)
{
	int error = 0;
	const char *branch = NULL;
	git_reference *head = NULL;

	error = git_repository_head(&head, repo);
	if (GIT_EUNBORNBRANCH == error || GIT_ENOTFOUND == error) {
		branch = NULL;
	}
	else if (!error) {
		branch = git_reference_shorthand(head);
	}
	else {
		print_git_error(error);
	}

	fprintf(stderr, "Branch: %s\n", branch ? branch : "HEAD (no branch)");

	git_reference_free(head);
}

void show_status(git_repository * const repo)
{
	int error;
	git_status_list *status = NULL;
	git_status_options statusopt = GIT_STATUS_OPTIONS_INIT;

	statusopt.show = GIT_STATUS_SHOW_INDEX_AND_WORKDIR;
	statusopt.flags = GIT_STATUS_OPT_INCLUDE_UNTRACKED;

	error = git_status_list_new(&status, repo, &statusopt);
	if (0 > error) {
		print_git_error(error);
	}

	if (NULL != status) {
		git_status_list_free(status);
	}
}

int main(int argc, char **argv)
{
	int error;
	git_repository *repo = NULL;

	if (2 > argc) {
		fprintf(stderr, "You need to supply a path to the repository\n");
		return 1;
	}

	error = git_libgit2_init();
	if (1 < error) {
		print_git_error(error);
		return error;
	}

	error = git_repository_open_ext(&repo, argv[1], 0, NULL);
	if (0 > error) {
		if (GIT_ENOTFOUND == error) {
			fprintf(stderr, "Could not find repo\n");
		}
		else {
			print_git_error(error);
		}

		return error;
	}
	else {
		fprintf(stderr, "We are in a git repo\n");
	}

	show_branch(repo);
	// TODO: Submodules
	show_status(repo);

	if (git_repository_is_bare(repo)) {
		fprintf(stderr, "%s: Cannot get status on bare repository\n", argv[0]);
	}
	else if (NULL != repo) {
		fprintf(stderr, "Repo workdir: %s\n", git_repository_workdir(repo));
	}


	if (NULL != repo) {
		git_repository_free(repo);
	}

	while(0 < git_libgit2_shutdown()) {};

	return 0;
}
