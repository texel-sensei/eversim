#include "core/input/inputhandler.h"

#include <catch.hpp>
#include <iostream>
using namespace eversim::core::input;

TEST_CASE("increment_simple", "[input][inputhandler]") {
	InputHandler handler("../resources/inputmaps/contexts.json");

	handler.push_context("game");

	int a = 0;

	handler.get_context("game")->register_function(
		InputConstants::button::JUMP,
		[&]() { a++; }
	);

	//press button 5 times
	SDL_Event event;
	event.type = SDL_JOYBUTTONDOWN;
	event.cbutton.button = SDL_CONTROLLER_BUTTON_A;

	for (size_t i = 0; i < 5; ++i)
		handler.handle_event(event);

	REQUIRE(a == 5);
}

TEST_CASE("increment_simple_mapped", "[input][inputhandler]") {
	InputHandler handler("../resources/inputmaps/contexts.json");

	handler.push_context("game");

	int a = 0;

	handler.get_context("game")->register_function(
		InputConstants::button::JUMP,
		[&]() { a++; }
	);

	//press button 5 times
	InputEvent event = InputEvent::create_button(
		RawInputConstants::event_type::BUTTON_DOWN,
		RawInputConstants::button::GAMEPAD_BUTTON_A
	);

	for (size_t i = 0; i < 5; ++i)
		handler.handle_event(event);

	REQUIRE(a == 5);
}