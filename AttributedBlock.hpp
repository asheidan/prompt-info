#ifndef __ATTRIBUTED_BLOCK_HPP__
#define __ATTRIBUTED_BLOCK_HPP__

#include <ostream>
#include <string>

typedef enum {
	AttributedBlockRenderAnsi16Color,
	AttributedBlockRenderAnsi256Color,
	AttributedBlockRenderZSH
} AttributedBlockRenderType;


extern AttributedBlockRenderType AttributedBlockRendering;

typedef int color_t;

class AttributedBlock {
public:
	AttributedBlock() {}

	AttributedBlock(const AttributedBlock &other);

	AttributedBlock(const std::string &content,
					const color_t foreground = -1, const color_t background = -1);

	size_t length() const
	{
		return content.length();
	}

	std::string content;
	color_t foreground;
	color_t background;
};

typedef std::ostream& (*AttributedBlockRenderer)(std::ostream &, const AttributedBlock&);

std::ostream& operator<<(std::ostream &os, const AttributedBlock & str);

#endif
