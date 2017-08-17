#include "editor/imgui/bar_graph.h"
#include <vector>
#include <imgui/imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui/imgui_internal.h>

using namespace std;

namespace ImGui {
	namespace {
		struct data_point{
			float value;
			string text;
			ImU32 color;
		};

		struct graph_data {
			float sum;
			vector<data_point> points;
		} g_data;

		bool in_graph_mode = false;
	}

	void BeginBarGraph()
	{
		assert(!in_graph_mode && "BeginBarGraph() can't be nested!");
		g_data.sum = 0.f;
		g_data.points.clear();
		in_graph_mode = true;
	}

	void BarGraphInputValue(float val, std::string const& tooltip)
	{
		assert(in_graph_mode && "BarGraphInputValue() outside of Begin/End block!");
		auto c = GetColorU32(ImGuiCol_Text);
		g_data.sum += val;
		g_data.points.push_back({ val, tooltip, c });
	}


	void EndBarGraph(ImVec2 size_arg)
	{
		assert(in_graph_mode && "EndBarGraph() without Begin!");
		in_graph_mode = false;

		auto* window = GetCurrentWindow();
		if (window->SkipItems)
			return;

		auto const& dat = g_data;

		auto& context = *GImGui;
		auto const& style = context.Style;

		auto pos = window->DC.CursorPos;
		auto bb = ImRect(
			pos, pos + CalcItemSize(size_arg, CalcItemWidth(), context.FontSize + style.FramePadding.y *2.f)
		);
		
		ItemSize(bb, style.FramePadding.y);
		if (!ItemAdd(bb, nullptr))
			return;
		RenderFrame(bb.Min, bb.Max, GetColorU32(ImGuiCol_FrameBg), true, style.FrameRounding);
		bb.Reduce(ImVec2(window->BorderSize, window->BorderSize));

		const auto full_width = bb.Max.x - bb.Min.x;
		const auto full_height = bb.Max.y - bb.Min.y;
		auto start = bb.Min;
		for(auto& p : dat.points)
		{
			const auto fraction = ImSaturate(p.value / dat.sum);
			const auto fill_br = ImVec2(fraction * full_width, full_height);

			const auto end = start + fill_br;
			RenderFrame(start, end, p.color, false, style.FrameRounding);
			if(IsItemHovered())
			{
				BeginTooltip();
					PushTextWrapPos(450.f);
					PushStyleColor(ImGuiCol_Text, ColorConvertU32ToFloat4(p.color));
					Text("%s: %.1f (%.0f%%)", p.text.c_str(), p.value, fraction*100);
					PopStyleColor();
					PopTextWrapPos();
				EndTooltip();
			}
			
			start.x += fill_br.x;
		}
	}
}
