CPPFLAGS = -Ilibgit2-0.23.1/include
CXXFLAGS = -Wall -Wextra -g -Llibgit2-0.23.1/build -lgit2

TARGET = prompt

default: $(TARGET)

test: $(TARGET)
	time ./$< $(shell pwd)

clean:
	$(RM) prompt
