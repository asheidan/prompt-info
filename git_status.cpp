#include <iostream>
#include <string>
#include <stdio.h>

#include <git2/diff.h>
#include <git2/errors.h>
#include <git2/global.h>
#include <git2/repository.h>
#include <git2/strarray.h>
#include <git2/status.h>

// https://libgit2.github.com/libgit2/ex/HEAD/status.html

int print_git_error(int error)
{
	const git_error *e = giterr_last();
	fprintf(stderr, "Error: %d/%d: %s\n", error, e->klass, e->message);

	return error;
}

std::string show_branch(git_repository * const repo)
{
	int error = 0;
	const char *branch = NULL;
	git_reference *head = NULL;
	std::string result;

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

	if (branch) {
		result = branch;
	}
	else {
		result = "(no branch)";
	}
	//fprintf(stdout, "%s", branch ? branch : "(no branch)");

	git_reference_free(head);

	return result;
}

std::string show_status(git_repository * const repo)
{
	int error;
	git_status_list *status = NULL;
	git_status_options statusopt = GIT_STATUS_OPTIONS_INIT;
	size_t status_count = 0;
	const git_status_entry *s;
	unsigned int repo_status = 0u;
	char istatus, wstatus;
	std::string result = ":  ";



	statusopt.show = GIT_STATUS_SHOW_INDEX_AND_WORKDIR;
	statusopt.flags = GIT_STATUS_OPT_INCLUDE_UNTRACKED;

	error = git_status_list_new(&status, repo, &statusopt);
	if (0 > error) {
		print_git_error(error);
	}

	status_count = git_status_list_entrycount(status);

	for (size_t i = 0; i < status_count; ++i) {
		s = git_status_byindex(status, i);

		if (GIT_STATUS_CURRENT == s->status) {
			continue;
		}

		repo_status |= s->status;
	}

	istatus = wstatus = ' ';

	if (repo_status & GIT_STATUS_INDEX_NEW) {
		istatus = 'A';
	}
	if (repo_status & GIT_STATUS_INDEX_MODIFIED) {
		istatus = 'M';
	}
	if (repo_status & GIT_STATUS_INDEX_DELETED) {
		istatus = 'D';
	}
	if (repo_status & GIT_STATUS_INDEX_RENAMED) {
		istatus = 'R';
	}
	if (repo_status & GIT_STATUS_INDEX_TYPECHANGE) {
		istatus = 'T';
	}


	if (repo_status & GIT_STATUS_WT_NEW) {
		if (istatus == ' ') {
			istatus = '?';
		}
		wstatus = '?';
	}
	if (repo_status & GIT_STATUS_WT_MODIFIED) {
		wstatus = 'M';
	}
	if (repo_status & GIT_STATUS_WT_DELETED) {
		wstatus = 'D';
	}
	if (repo_status & GIT_STATUS_WT_RENAMED) {
		wstatus = 'R';
	}
	if (repo_status & GIT_STATUS_WT_TYPECHANGE) {
		wstatus = 'T';
	}

	if (repo_status & GIT_STATUS_IGNORED) {
		istatus = '!';
		wstatus = '!';
	}

	result.replace(1,1,1, istatus);
	result.replace(2,1,1, wstatus);

	if (NULL != status) {
		git_status_list_free(status);
	}

	return result;
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
		//fprintf(stderr, "We are in a git repo\n");
	}

	std::cout << show_branch(repo);
	// TODO: Submodules
	std::cout << show_status(repo);

	if (git_repository_is_bare(repo)) {
		fprintf(stderr, "%s: Cannot get status on bare repository\n", argv[0]);
	}
	else if (NULL != repo) {
		//fprintf(stderr, "Repo workdir: %s\n", git_repository_workdir(repo));
	}

	if (NULL != repo) {
		git_repository_free(repo);
	}

	while(0 < git_libgit2_shutdown()) {};

	fprintf(stdout, "\n");

	return 0;
}
