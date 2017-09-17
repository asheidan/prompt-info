#include "AttributedBlock.hpp"

AttributedBlockRenderType AttributedBlockRendering = AttributedBlockRenderAnsi256Color;

AttributedBlock::AttributedBlock(const AttributedBlock &other)
	: content(other.content), foreground(other.foreground), background(other.background)
{}

AttributedBlock::AttributedBlock(const std::string &content,
								 const color_t foreground, const color_t background)
	: content(content), foreground(foreground), background(background)
{}


std::ostream& AttributedBlockZSHRenderer(std::ostream &os, const AttributedBlock & str)
{
	bool has_attribute = str.foreground != 0x0 || str.background != 0x0;

	if (has_attribute) {
		os << "%{";

		if (0x0 <= str.foreground && str.foreground <= 0xFF) {
			os << "%F{" << str.foreground << "}";
		}
		if (0x0 <= str.background && str.background <= 0xFF) {
			os << "%K{" << str.background << "}";
		}

		os << "%}";
	}

	os << str.content;

	return os;
}

std::ostream& AttributedBlockAnsi256ColorRenderer(std::ostream &os, const AttributedBlock & str)
{
	if (0x0 <= str.foreground && str.foreground <= 0xFF) {
		os << "\x1B[38;5;" << str.foreground << "m";
	}
	if (0x0 <= str.background && str.background <= 0xFF) {
		os << "\x1B[48;5;" << str.background << "m";
	}

	os << str.content;

	return os;
}

std::ostream& operator<<(std::ostream &os, const AttributedBlock & str)
{
	return (AttributedBlockZSHRenderer)(os, str);
}
