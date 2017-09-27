#include "editor/windows/physicsbody_creator.h"

#include "core/utility/helper.h"

#include <imgui/imgui_internal.h>

#include <boost/format.hpp>

#include <easylogging++.h>

using namespace std;
using namespace eversim::core;

namespace eversim { namespace editor { namespace windows {

	physicsbody_creator::physicsbody_creator(std::string const& name, bool visible) :
		base_window(name,visible)
	{
		clear();
	}

	void physicsbody_creator::clear()
	{
		for (int i = 0; i < 32; i++)
			body_name[i] = '\0';
	}

	void physicsbody_creator::draw_content()
	{
		ImGui::InputText("New Particle Name", body_name, sizeof(body_name));

		if (ImGui::Button("Add particle"))
		{

		}

		if (ImGui::Button("Add constraint"))
		{

		}

		if (ImGui::Button("Create body"))
		{
			clear();
			LOG(INFO) << "create body";
		}
	}

/*	void physicsbody_creator::draw_menu()
	{
	}*/

	/*void physicsbody_creator::begin_window()
	{
	}*/

}}}
