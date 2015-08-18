#ifndef __ATTRIBUTED_STRING_HPP__
#define __ATTRIBUTED_STRING_HPP__

#include <vector>

#include "AttributedBlock.hpp"

class AttributedString {
public:
	void append(AttributedBlock &s)
	{
		blocks.push_back(s);
	}

	size_t length() const
	{
		size_t
			length = 0;

		std::vector<AttributedBlock>::const_iterator it;
		for (it = blocks.cbegin(); it < blocks.cend(); ++it) {
			length += it->length();
		}

		return length;
	}

	std::vector<AttributedBlock> blocks;
};

std::ostream& operator<<(std::ostream &os, const AttributedString & text)
{
	std::vector<AttributedBlock>::const_iterator it;
	for (it = text.blocks.cbegin(); it < text.blocks.cend(); ++it) {
		os << *it;
	}
	return os;
}

#endif
