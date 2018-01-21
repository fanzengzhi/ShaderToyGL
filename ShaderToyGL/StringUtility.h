#pragma once

#include <vector>
#include <map>
#include <fstream>
#include "StringUtility.h"

std::string& trim(std::string &s)
{
	if (s.empty())
	{
		return s;
	}

	s.erase(0, s.find_first_not_of(" "));
	s.erase(s.find_last_not_of(" ") + 1);
	return s;
}

void split(std::string& s, std::string& delim, std::vector< std::string >* ret)
{
	size_t last = 0;
	size_t index = s.find_first_of(delim, last);
	while (index != std::string::npos)
	{
		ret->push_back(trim(s.substr(last, index - last)));
		last = index + 1;
		index = s.find_first_of(delim, last);
	}
	if (index - last > 0)
	{
		ret->push_back(trim(s.substr(last, index - last)));
	}
}

std::map<std::string, int> getTextures(const std::string& path)
{
	std::ifstream inStream(path, std::ios::in);

	std::map<std::string, int> textureMaps;
	if (inStream.is_open())
	{
		char buffer[1024];
		inStream.getline(buffer, 1024, '\n');

		std::string lineContent(buffer);
		trim(lineContent);

		lineContent.erase(0, 2); // erase "//"

		std::vector<std::string> textures;
		split(lineContent, std::string(";"), &textures);

		for (auto& texure : textures)
		{
			std::vector<std::string> texInfos;

			split(texure, std::string("="), &texInfos);

			if (texInfos.size() < 2) continue; // jump over wrong contents

			int index = atoi(&texInfos[0][texInfos[0].length() - 1]);
			textureMaps.insert(std::make_pair(texInfos[1], index));
		}

	}

	return textureMaps;
}