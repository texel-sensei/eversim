#include "core/input/inputhandler.h"

#include <catch.hpp>
#include <iostream>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

using namespace eversim::core::input;

const auto simple_file = 
R"#delim#({
"game": {
		"JUMP": ["GAMEPAD_BUTTON_A", "BUTTON"],
		"MENU" : ["GAMEPAD_BUTTON_START", "BUTTON"],
		"DUCK" : ["GAMEPAD_BUTTON_X", "STATE"],
		"STEER_X" : ["GAMEPAD_ANALOGUE_LEFT_STICK_X", "RANGE"],
		"STEER_Y" : ["GAMEPAD_ANALOGUE_LEFT_STICK_Y", "RANGE"],
		"FART_LEFT" : ["GAMEPAD_BUTTON_LB", "BUTTON"],
		"FART_RIGHT" : ["GAMEPAD_BUTTON_RB", "BUTTON"],
		"DLEFT" : ["GAMEPAD_BUTTON_DPAD_LEFT", "BUTTON"],
		"DRIGHT" : ["GAMEPAD_BUTTON_DPAD_RIGHT", "BUTTON"],
		"DUP" : ["GAMEPAD_BUTTON_DPAD_UP", "BUTTON"],
		"DDOWN" : ["GAMEPAD_BUTTON_DPAD_DOWN", "BUTTON"]
},
"menu": {
		"BACK": ["GAMEPAD_BUTTON_B", "BUTTON"],
			"CONFIRM" : ["GAMEPAD_BUTTON_A", "BUTTON"]
	}
})#delim#";
const std::string sf(simple_file);

const auto harder_file =
R"#delim#({
"game": {
		"JUMP": ["GAMEPAD_BUTTON_A", "BUTTON"],
		"MENU": ["GAMEPAD_BUTTON_A", "BUTTON"]
}})#delim#";
const std::string hf(harder_file);

TEST_CASE("increment_simple", "[input][inputhandler]") {

	std::istringstream iss(sf);

	InputHandler handler(iss);

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

	for (size_t i = 0; i < 5; ++i) {
		handler.handle_event(event);
		handler.execute();
	}

	REQUIRE(a == 5);
}

TEST_CASE("increment_two_contexts", "[input][inputhandler]") {

	std::istringstream iss(sf);

	InputHandler handler(iss);

	handler.push_context("game");
	handler.push_context("menu");

	int a = 0;

	handler.get_context("game")->register_function(
		InputConstants::button::JUMP,
		[&]() { a++; }
	);

	//press button 5 times
	SDL_Event event;
	event.type = SDL_JOYBUTTONDOWN;
	event.cbutton.button = SDL_CONTROLLER_BUTTON_A;

	for (size_t i = 0; i < 5; ++i) {
		handler.handle_event(event);
		handler.execute();
	}

	handler.pop();

	handler.handle_event(event);
	handler.execute();

	REQUIRE(a == 1);
}

TEST_CASE("increment_harder", "[input][inputhandler]") {

	std::istringstream iss(hf);

	InputHandler handler(iss);

	handler.push_context("game");

	int a = 0;
	int b = 0;

	handler.get_context("game")->register_function(
		InputConstants::button::JUMP,
		[&]() { a++; }
	);

	handler.get_context("game")->register_function(
		InputConstants::button::MENU,
		[&]() { b++; }
	);

	//press button 5 times
	SDL_Event event;
	event.type = SDL_JOYBUTTONDOWN;
	event.cbutton.button = SDL_CONTROLLER_BUTTON_A;


	handler.handle_event(event);
	handler.execute();


	REQUIRE(b == 1);
	REQUIRE(a == 1);
	REQUIRE(a == b);
}

TEST_CASE("increment_simple_mapped", "[input][inputhandler]") {
	std::istringstream iss(sf);

	InputHandler handler(iss);

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

	for (size_t i = 0; i < 5; ++i) {
		handler.handle_event(event);
		handler.execute();
	}

	REQUIRE(a == 5);
}