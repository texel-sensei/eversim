#include "core/rendering/multibuffer.h"

#include <exception>
#include <iostream>

using namespace std;

namespace eversim::core::rendering {
	Multibuffer::Multibuffer(const string& name) : name(name)
	{
	}

	Multibuffer::~Multibuffer()
	{
		glDeleteBuffers(metadata.size(), VboIds);
		glDeleteVertexArrays(1, &VaoId);
	}

	void Multibuffer::attach(const std::vector<glm::fvec2>& data)
	{
		arrays2.push_back(data);
		metadata.emplace_back(2, arrays2.size() - 1);
	}

	void Multibuffer::attach(std::vector<glm::fvec2>&& data)
	{
		arrays2.push_back({});
		std::swap(arrays2.back(), data);
		metadata.emplace_back(2, arrays2.size() - 1);
	}

	void Multibuffer::attach(const std::vector<glm::fvec3>& data)
	{
		arrays3.push_back(data);
		metadata.emplace_back(3,arrays3.size()-1);
	}

	void Multibuffer::attach(std::vector<glm::fvec3>&& data)
	{
		arrays3.push_back({});
		std::swap(arrays3.back(),data);
		metadata.emplace_back(3,arrays3.size() - 1);
	}

	void Multibuffer::attach(const std::vector<glm::fvec4>& data)
	{
		arrays4.push_back(data);
		metadata.emplace_back(4, arrays4.size() - 1);
	}

	void Multibuffer::attach(std::vector<glm::fvec4>&& data)
	{
		arrays4.push_back({});
		std::swap(arrays4.back(), data);
		metadata.emplace_back(4, arrays4.size() - 1);
	}

	void Multibuffer::create()
	{
		if (metadata.size() == 0)
			LOG(ERROR) << "Multibuffer can't be created with empty data";

		glGenVertexArrays(1, &VaoId);
		glBindVertexArray(VaoId);

		VboIds = new GLuint[metadata.size()];

		glGenBuffers(metadata.size(), VboIds);

		glBindVertexArray(0);

		LOG(INFO) << "Created Multibuffer with " << metadata.size() << " part(s)";
		for (size_t i = 0; i < metadata.size(); ++i)
		{
			size_t size = 0;
			string type = "none";
			auto& md = metadata.at(i);
			switch (md.first) {
				case 2:
					size = arrays2.at(md.second).size();
					type = "glm::fvec2";
					break;
				case 3:
					size = arrays3.at(md.second).size();
					type = "glm::fvec3";
					break;
				case 4:
					size = arrays4.at(md.second).size();
					type = "glm::fvec4";
					break;
				default:
					LOG(ERROR) << "Misconfigured multibuffer";
					break;
			}
			LOG(INFO) << "Part " << i << " with " << size << " elements of type " <<
				type;
		}

	}

	void Multibuffer::set_draw_mode(GLenum type, GLint first, GLint count)
	{
		this->type = type;
		this->first = first;
		this->count = count;
	}

	void Multibuffer::bind() const
	{
		glBindVertexArray(VaoId);
	}

	void Multibuffer::draw() const
	{
		glDrawArrays(type, first, count);
	}

	void Multibuffer::bind_and_draw() const
	{
		bind(); draw();
	}

	void Multibuffer::upload()
	{
		glBindVertexArray(VaoId);

		for (size_t i = 0; i < metadata.size(); i++)
		{
			glBindBuffer(GL_ARRAY_BUFFER, VboIds[i]);
			auto& md = metadata.at(i);

			void* ptr = nullptr;
			size_t size = 0;
			switch (md.first) {
				case 2:
					{
						auto &data = arrays2.at(md.second);
						ptr = (void*)data.data();
						size = data.size() * sizeof(glm::fvec2);
					}
					break;
				case 3:
					{
						auto &data = arrays3.at(md.second);
						ptr = (void*)data.data();
						size = data.size() * sizeof(glm::fvec3);
					}
					break;
				case 4:
					{
						auto &data = arrays4.at(md.second);
						ptr = (void*)data.data();
						size = data.size() * sizeof(glm::fvec4);
					}
					break;
				default:
					LOG(ERROR) << "Misconfigured multibuffer";
					break;
			}

			glBufferData(GL_ARRAY_BUFFER,size, ptr, GL_STATIC_DRAW);
			glVertexAttribPointer(i, md.first , GL_FLOAT, GL_FALSE, 0, 0);

			glEnableVertexAttribArray(i);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}

		glBindVertexArray(0);
	}

}
