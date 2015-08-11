CXXFLAGS = -Wall -Wextra

test: perf-c++
	time ./$<
