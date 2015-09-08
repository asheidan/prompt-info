CPPFLAGS = -Ilibgit2-0.23.1/include
CXXFLAGS = -std=c++98 -Wall -Wextra -g -Llibgit2-0.23.2/build -lgit2

LIBGIT = libgit2-0.23.2/build/libgit2.so

TARGET = prompt

default: $(TARGET)

test: $(TARGET)
	time ./$< $(shell pwd)

OBJS = prompt.o

$(TARGET): $(OBJS) $(LIBGIT)
	$(LINK.cc) -o $@ $<

clean:
	$(RM) prompt

libgit2-v0.23.2.tar.gz:
	curl -L -o $@ https://github.com/libgit2/libgit2/archive/v0.23.2.tar.gz

libgit2-0.23.2: libgit2-v0.23.2.tar.gz
	tar xzf $<

libgit2-0.23.2/build: libgit2-0.23.2
	mkdir -p $@

libgit2-0.23.2/build/CMakeFiles: libgit2-0.23.2/build
	cd $< && cmake -DBUILD_CLAR=OFF ..

libgit2-0.23.2/build/libgit2.so: libgit2-0.23.2/build/CMakeFiles
	cd $< && cd .. && cmake --build .
