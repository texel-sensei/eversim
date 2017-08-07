#include "editor/windows/physics_inspector.h"

#include "core/physics/physics_manager.h"
#include "core/utility/helper.h"

#include <imgui/imgui_internal.h>

#include <boost/format.hpp>

using namespace std;
using namespace eversim::core;

namespace eversim { namespace editor { namespace windows {

	void physics_inspector::draw_content()
	{
		display_body_info();
		display_particle_info();
	}

	void physics_inspector::display_particle_info()
	{
		auto const& particles = physics->get_particles();
		auto name = (boost::format("Particles: %d/%d###cat") % particles.size() % particles.capacity()).str();
		if (!ImGui::CollapsingHeader(name.c_str()))
			return;
		ImGui::Indent();
		auto begin = particles.data();
		for (auto const& p : particles)
		{
			auto col = p.is_alive() ? ImVec4(1, 1, 1, 1) : ImVec4(0.6, 0.6, 0.6, 1);
			ImGui::PushStyleColor(ImGuiCol_Text, col);
			ImGui::Text("Particle #%d", &p - begin);
			ImGui::SameLine(300);
			ImGui::Text("Body %s", body_name(p.owner).c_str());

			ImGui::PopStyleColor();
		}
		ImGui::Unindent();
	}

	void physics_inspector::display_body_info()
	{
		ImGui::PushID("bodies_category");
		const auto pop_id = utility::finally(ImGui::PopID);

		auto n = physics->get_num_bodies();
		auto name = "Bodies: "s + to_string(n) + "###body_cat";
		if (!ImGui::CollapsingHeader(name.c_str()))
			return;

		ImGui::Indent();
		if (ImGui::Button("Cleanup bodies"))
		{
			physics->cleanup_dead_bodies();
		}
		if (selected_body)
		{
			const auto changed = ImGui::InputText("Name", name_buffer, sizeof(name_buffer));

			if (changed)
				body_names[selected_body] = name_buffer;
			ImGui::SameLine();
			if (ImGui::Button("Delete"))
			{
				physics->remove_body(selected_body);
				selected_body = nullptr;
			}
		}
		for (auto& b : physics->get_bodies())
		{
			ImGui::PushID(&b);
			bool selected = (&b == selected_body);
			const auto clicked = ImGui::Selectable(
				body_name(&b).c_str(),
				&selected,
				ImGuiSelectableFlags_AllowDoubleClick
			);

			if (clicked)
			{
				if (selected_body == &b)
					selected_body = nullptr;
				else
				{
					selected_body = &b;
					const auto text = body_name(selected_body);
					copy(text.begin(), text.end(), begin(name_buffer));
					name_buffer[text.size()] = '\0';
				}
			}
			ImGui::PopID();
		}
		ImGui::Unindent();
	}

	string physics_inspector::body_name(physics::body const* b) const
	{
		const auto it = body_names.find(b);
		if (it == body_names.end())
		{
			return (boost::format("%p") % b).str();
		}
		return it->second;
	}

}}}
