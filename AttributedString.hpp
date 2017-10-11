#ifndef __ATTRIBUTED_STRING_HPP__
#define __ATTRIBUTED_STRING_HPP__

#include <vector>
#include <ostream>

#include "AttributedBlock.hpp"

class AttributedString {
public:
	void append(AttributedBlock &b)
	{
		blocks.push_back(b);
	}

	size_t length() const
	{
		size_t
			length = 0;

		std::vector<AttributedBlock>::const_iterator it;
		for (it = blocks.begin(); it < blocks.end(); ++it) {
			length += it->length();
		}

		return length;
	}

	std::vector<AttributedBlock> blocks;
};

std::ostream& operator<<(std::ostream &os, const AttributedString & text)
{
	std::vector<AttributedBlock>::const_iterator it;
	for (it = text.blocks.begin(); it < text.blocks.end(); ++it) {
		os << *it;
	}
	return os;
}

std::ostream& operator<<(std::ostream &os, const std::vector<AttributedString> & strings)
{
	std::vector<AttributedString>::const_iterator it = strings.begin();

	if (it < strings.end()) {
		os << *it;
	}
	for (++it; it < strings.end(); ++it) {
		os << " ";
		os << *it;
	}

	return os;
}

size_t length(std::vector<AttributedString> &v)
{
	size_t result = 0;
	std::vector<AttributedString>::const_iterator it = v.begin();

	if (it < v.end()) {
		result += it->length();
	}
	for (++it; it < v.end(); ++it) {
		result += it->length() + 1;
	}

	return result;
}
#endif
