#ifndef __GIT_STATUS_HPP
#define __GIT_STATUS_HPP

#include <string>
#include <exception>
#include <git2/repository.h>

class GitInitializationException : public std::exception {
public:
	GitInitializationException(const char * const message)
		: message(message)
	{}
	~GitInitializationException() throw()
	{}

	virtual const char* what() const throw()
	{
		return message;
	}

private:
	const char * message;
};

class GitRepo {
public:
	GitRepo(const char * const path);
	~GitRepo();

	std::string branch() const;
	std::string status() const;
	std::string workdir() const;

protected:
	git_repository *repo;
	bool is_bare;

};

#endif
