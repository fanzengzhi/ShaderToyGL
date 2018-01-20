#pragma once

#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
	unsigned int ID;
	// constructor generates the shader on the fly
	// ------------------------------------------------------------------------
	Shader(const char* fragmentPath)
	{
		// 1. retrieve the vertex/fragment source code from filePath
		std::string vertexCode;
		std::string fragmentCode = "";
		std::ifstream vShaderFile;
		std::ifstream fShaderFile;
		// ensure ifstream objects can throw exceptions:
		vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		// Header
		/*
		uniform vec3      iResolution;           // viewport resolution (in pixels)
		uniform float     iGlobalTime;           // shader playback time (in seconds)
		uniform float     iChannelTime[4];       // channel playback time (in seconds)
		uniform vec3      iChannelResolution[4]; // channel resolution (in pixels)
		uniform vec4      iMouse;                // mouse pixel coords. xy: current (if MLB down), zw: click
		uniform samplerXX iChannel0..3;          // input channel. XX = 2D/Cube
		uniform vec4      iDate;                 // (year, month, day, time in seconds)
		uniform float     iSampleRate;           // sound sample rate (i.e., 44100)
		*/
		fragmentCode += "#version 330 core \n\
uniform float time; \n \
uniform vec3 iResolution; \n\
uniform float iTime; \n \
uniform float iTimeDelta; \n \
//uniform float iFrame; \n \
//uniform float iChannelTime[4];  \n \
uniform vec4 iMouse; \n \
//uniform vec4 iDate; \n \
//uniform float iSampleRate; \n \
//uniform vec3 iChannelResolution[4]; \n \
out vec4 FragColor; \n \
in vec3 ourColor; \n \
in vec2 TexCoord; \n \
uniform sampler2D iChannel0; \n \
uniform sampler2D iChannel1; \n \
uniform sampler2D iChannel2; \n \
uniform sampler2D iChannel3; \n \
uniform sampler2D iChannel4; \n \
uniform sampler2D iChannel5; \n \
uniform sampler2D iChannel6; \n ";

		try
		{
			// open files
			//vShaderFile.open(vertexPath);
			fShaderFile.open(fragmentPath);
			std::stringstream vShaderStream, fShaderStream;
			// read file's buffer contents into streams
			//vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();
			// close file handlers
			//vShaderFile.close();
			fShaderFile.close();
			// convert stream into string
			//vertexCode = vShaderStream.str();
			fragmentCode += fShaderStream.str();
		}
		catch (std::ifstream::failure e)
		{
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
		}

		vertexCode = " \n\
#version 330 core \n\
layout(location = 0) in vec3 aPos;  \n\
layout(location = 1) in vec3 aColor;  \n\
layout(location = 2) in vec2 aTexCoord; \n\
\n\
out vec3 ourColor; \n\
out vec2 TexCoord; \n\
\n\
void main() \n\
{ \n\
	gl_Position = vec4(aPos, 1.0); \n\
	ourColor = aColor; \n\
	TexCoord = aTexCoord; \n\
} \n\
";
		const char* vShaderCode = vertexCode.c_str();

		fragmentCode += "\n\
void main() \
{ \
	vec2 fragCoord = TexCoord * iResolution.xy;  \
	vec4 fragcolor; \
	mainImage(fragcolor, fragCoord); \
	FragColor = fragcolor; \
}\n";
		const char * fShaderCode = fragmentCode.c_str();




		// 2. compile shaders
		unsigned int vertex, fragment;
//		int success;
		//char infoLog[512];
		// vertex shader
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, NULL);
		glCompileShader(vertex);
		checkCompileErrors(vertex, "VERTEX");
		// fragment Shader
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, NULL);
		glCompileShader(fragment);
		checkCompileErrors(fragment, "FRAGMENT");
		// shader Program
		ID = glCreateProgram();
		glAttachShader(ID, vertex);
		glAttachShader(ID, fragment);
		glLinkProgram(ID);
		checkCompileErrors(ID, "PROGRAM");
		// delete the shaders as they're linked into our program now and no longer necessary
		glDeleteShader(vertex);
		glDeleteShader(fragment);
	}
	// activate the shader
	// ------------------------------------------------------------------------
	void use()
	{
		glUseProgram(ID);
	}
	// utility uniform functions
	// ------------------------------------------------------------------------
	void setBool(const std::string &name, bool value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
	}
	// ------------------------------------------------------------------------
	void setInt(const std::string &name, int value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	}
	// ------------------------------------------------------------------------
	void setFloat(const std::string &name, float value) const
	{
		glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	}
	// ------------------------------------------------------------------------
	void setVec2(const std::string &name, float* value)
	{
		glUniform2f(glGetUniformLocation(ID, name.c_str()), value[0], value[1]);
	}
	// ------------------------------------------------------------------------
	void setVec3(const std::string &name, float* value)
	{
		glUniform3f(glGetUniformLocation(ID, name.c_str()), value[0], value[1], value[2]);
	}
	// ------------------------------------------------------------------------
	void setVec4(const std::string &name, float* value)
	{
		glUniform4f(glGetUniformLocation(ID, name.c_str()), value[0], value[1], value[2], value[3]);
	}

private:
	// utility function for checking shader compilation/linking errors.
	// ------------------------------------------------------------------------
	void checkCompileErrors(unsigned int shader, std::string type)
	{
		int success;
		char infoLog[1024];
		if (type != "PROGRAM")
		{
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			}
		}
		else
		{
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			}
		}
	}
};
#endif