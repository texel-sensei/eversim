#pragma once

#include <gl/glew.h>
#include <easylogging++.h>

#include <fstream>
#include <string>
#include <vector>
#include <exception>
#include <iostream>

class AttachableShader
{
protected:
	GLuint id;
public:
	const std::string name;
	bool created = false;
	AttachableShader(const std::string& name) : name(name)
	{
	}
	virtual ~AttachableShader()
	{
	}

	virtual void create(const std::string& filename) = 0;

	GLuint getID() const { return id; }

	void compile(const std::string& filename)
	{
		LOG(INFO) << "create " << filename << std::endl;
		std::ifstream file(filename);
		if (!file) throw std::exception("Could not open source file for compilation");
		std::string sourcecode("");
		std::string line;

		
		while (std::getline(file, line)) {
			sourcecode += line + '\n';
		}
		const GLchar* tmp_gl_source = sourcecode.c_str();
		
		glShaderSource(id, 1, &tmp_gl_source, NULL);
		//TODO error
		glCompileShader(id);
		//TODO error
		LOG(INFO) << "\tcreate successfull";

	}

	std::string GetShaderInfoLog () const
	{
		GLsizei length;
		GLsizei size = 4096;
		std::vector<char> infoLog(size);
		glGetShaderInfoLog(id, size, &length, infoLog.data());
		std::string result("");
		for (size_t i = 0; i < length; ++i)
		{
			result += infoLog.at(i);
		}
		result.back() = '\0';
		return result;
	}
};

