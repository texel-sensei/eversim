#pragma once

#include "editor/core/base_window.h"
#include "core/rendering/framebuffer.h"

namespace eversim { namespace editor { namespace windows {

	class game_window : public core::base_window {
	public:
		explicit game_window(
			std::string const& name = "game", bool visible = true
		);

		bool has_menu() const override { return true; }
		void draw_menu() override;

		void bind() const;
		void unbind() const;

	protected:
		void begin_window() override;
		void draw_content() override;
	private:
		eversim::core::rendering::Framebuffer fb;
	};

}}}
