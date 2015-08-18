#ifndef __ATTRIBUTED_BLOCK_HPP__
#define __ATTRIBUTED_BLOCK_HPP__

#include <ostream>
#include <string>

typedef int color_t;
class AttributedBlock {
public:
	AttributedBlock() {}

	AttributedBlock(const AttributedBlock &other)
		: content(other.content), foreground(other.foreground), background(other.background)
	{}

	AttributedBlock(const std::string &content,
				   const color_t foreground = -1, const color_t background = -1)
		: content(content), foreground(foreground), background(background)
	{}

	size_t length() const
	{
		return content.length();
	}

	std::string content;
	color_t foreground;
	color_t background;
};

std::ostream& operator<<(std::ostream &os, const AttributedBlock & str)
{
	if (0x0 <= str.foreground && str.foreground <= 0xFF) {
		os << "\x1B[38;5;" << str.foreground << "m";
	}
	if (0x0 <= str.background && str.background <= 0xFF) {
		os << "\x1B[48;5;" << str.background << "m";
	}

	os << str.content;

	/*
	if (str.reset) {
		os << "\x1B[0m";
	}
	*/

	return os;
}

#endif
