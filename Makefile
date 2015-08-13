CXXFLAGS = -Wall -Wextra -g

default: prompt

test: prompt
	./$<
