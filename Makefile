CPPFLAGS = -Ilibgit2-0.23.1/include
CXXFLAGS = -Wall -Wextra -g -Llibgit2-0.23.1/build -lgit2

default: prompt

test: prompt
	time ./$<

clean:
	$(RM) prompt
