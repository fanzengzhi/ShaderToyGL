#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include "stb_image.h"
#include "PathOperation.h"

struct ToyTexture
{
	typedef unsigned int uint;

	uint mWrapS;
	uint mWrapT;
	uint mMinFiler;
	uint mMagFilter;
	uint pixelFormat;

	uint texHandle;

	ToyTexture()
	{
		mWrapS = mWrapT = GL_REPEAT;
		mMinFiler = mMagFilter = GL_LINEAR;
		pixelFormat = GL_RGB;
		texHandle = -1;
	}

	~ToyTexture()
	{
		if (texHandle != -1)
		{
			glDeleteTextures(1, &texHandle);
		}
	}

	bool init()
	{
		glGenTextures(1, &texHandle);
		glBindTexture(GL_TEXTURE_2D, texHandle);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, mWrapS);	// set texture wrapping to GL_REPEAT (default wrapping method)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, mWrapT);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mMinFiler);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mMagFilter);

		return texHandle != -1;
	}

	bool setData(std::string imagePath, bool needMimMap = true)
	{
		stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
												// The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
		int width, height, nrChannels;
		//string imagePath = initPath + "\\test.jpg";
		unsigned char *data = stbi_load(imagePath.c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, pixelFormat, width, height, 0, pixelFormat, GL_UNSIGNED_BYTE, data);
			
			if (needMimMap)
			{
				glGenerateMipmap(GL_TEXTURE_2D);
			}
		}
		else
		{
			std::cout << "Failed to load texture:" << imagePath << "\n";
		}

		stbi_image_free(data);

		return true;
	}

	bool setTexIndex(int index)
	{
		glActiveTexture(GL_TEXTURE0 + index);
		glBindTexture(GL_TEXTURE_2D, texHandle);
		glActiveTexture(0);

		return true;
	}


};