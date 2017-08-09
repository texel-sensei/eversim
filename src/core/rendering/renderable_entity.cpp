#include "core/rendering/renderable_entity.h"
#include "core/rendering/drawcall_entity.h"

#include "core/utility/plattform.h"

#include <easylogging++.h>

using namespace std;
using namespace glm;

namespace eversim {	namespace core { namespace rendering {


	RenderableEntity::RenderableEntity(
		std::shared_ptr<ShaderProgram> program,
		std::shared_ptr<TextureBase> tex,
		std::weak_ptr<Multibuffer>  data,
		const entity_type t) :
		program(program),
		tex(tex),
		data(data),
		type(t)
	{}

	RenderableEntity::~RenderableEntity()
	{

		if (assigned_drawer.first >= 0 && !assigned_drawer.second.expired())
			assigned_drawer.second.lock()->remove_entity(assigned_drawer.first);
	}

	instanced_entity_information RenderableEntity::get_instanced_entity_information() const
	{
		instanced_entity_information ifo;
		get_instanced_entity_information(ifo);
		return ifo;
	}

	void RenderableEntity::get_instanced_entity_information(instanced_entity_information& ifo) const
	{
		ifo.set_M(M);
		ifo.set_texoffset(texoffset);
		ifo.set_texsize(texsize);
		ifo.set_spritesize(spritesize);
	}

	fmat3 RenderableEntity::get_M() const { return M; };

	weak_ptr<ShaderProgram> RenderableEntity::get_ShaderProgram() const { return program; };

	weak_ptr<TextureBase> RenderableEntity::get_Texture() const { return tex; };

	weak_ptr<Multibuffer> RenderableEntity::get_Multibuffer() const { return data; };

	void RenderableEntity::set_M(const fmat3& m)
	{
		M = m; 
		touch();
	};

	void RenderableEntity::set_ShaderProgram(std::shared_ptr<ShaderProgram> p)
	{
		program = p;
		touch();
	}
	
	void RenderableEntity::set_Texture(std::shared_ptr<TextureBase> p)
	{
		tex = p;
		touch();
	}

	void RenderableEntity::set_Texture(std::shared_ptr<TextureBase> p,
		const ivec2& offset, const ivec2& resolution)
	{
		tex = p;
		texoffset = offset;
		texsize = resolution;
		spritesize = p->get_resolution();
		touch();
	}

	void RenderableEntity::set_Multibuffer(std::shared_ptr<Multibuffer> p)
	{
		data = p;
		touch();
	}

	void RenderableEntity::set_Position(fvec2 pos)
	{
		M[2] = fvec3(pos,1.f);
		touch();
	}

	glm::fvec2  RenderableEntity::get_Position() const
	{
		return { M[2][0], M[2][1] };
	}

	void RenderableEntity::set_Scale(glm::fvec2 scale)
	{
		M[0] = { scale[0], 0, 0 };
		M[1] = { 0, scale[1], 0};
		touch();
	}

	glm::fvec2 RenderableEntity::get_Scale() const
	{
		return { M[0][0], M[1][1] };
	}

	void RenderableEntity::touch()
	{
		touched = true;
		auto idx = assigned_drawer.first;
		auto drawer_ptr = assigned_drawer.second;
		if(assigned_drawer.first >= 0 && !drawer_ptr.expired())
		{
			drawer_ptr.lock()->touch(idx);
		}
	}
	void RenderableEntity::untouch()
	{
		touched = false;
	};

	bool RenderableEntity::get_touched() const
	{
		return touched;
	}

	void RenderableEntity::set_Type(const entity_type t)
	{
		type = t;
	}

	entity_type RenderableEntity::get_Type() const
	{
		return type;
	}

	void RenderableEntity::set_Drawer(std::weak_ptr<DrawcallEntity> de,size_t idx)
	{
		assigned_drawer = make_pair(idx,de);
	}

	void RenderableEntity::set_Drawer(size_t idx)
	{
		assigned_drawer.first = idx;
	}

	std::weak_ptr<DrawcallEntity> RenderableEntity::get_Drawer() const
	{
		return assigned_drawer.second;
	}

	long long RenderableEntity::get_Drawer_idx() const
	{
		return assigned_drawer.first;
	}

}}}