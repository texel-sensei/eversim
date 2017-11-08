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
    "JUMP": {
      "type": "BUTTON",
      "keys": [ "GAMEPAD_BUTTON_A" ]
    },
    "BACK": {
      "type": "BUTTON",
      "keys": [ "GAMEPAD_BUTTON_DPAD_LEFT" ]
    }
  },
  "midjump": {
    "DOUBLEJUMP": {
      "type": "BUTTON",
      "keys": [ "GAMEPAD_BUTTON_A" ]
    }
  },
  "midjumpjumped": {
    "DROP": {
      "type": "BUTTON",
      "keys": [ "GAMEPAD_BUTTON_A" ]
    }
  },
 "menu":{
  "MENU" : {
      "type": "BUTTON",
      "keys": [ "GAMEPAD_BUTTON_A" ]
    }
 }
})#delim#";

const auto harder_file =
R"#delim#({
"game": {
    "JUMP": {
      "type": "BUTTON",
      "keys": [ "GAMEPAD_BUTTON_A" ]
    },
    "MENU": {
      "type": "BUTTON",
      "keys": [ "GAMEPAD_BUTTON_A" ]
    }
}})#delim#";

TEST_CASE("increment_simple", "[input][inputhandler]") {

	std::istringstream iss(simple_file);

	InputHandler handler(iss);

	handler.push_context("game");

	int a = 0;

	handler.get_context("game")->register_function_button(
		"JUMP",
		[&](InputContext&) { a++; }
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

	std::istringstream iss(simple_file);

	InputHandler handler(iss);

	handler.push_context("game");
	handler.push_context("menu");

	int a = 0;

	handler.get_context("game")->register_function_button(
		"JUMP",
		[&](InputContext&) { a++; }
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

TEST_CASE("two_buttons", "[input][inputhandler]") {

	std::istringstream iss(harder_file);

	InputHandler handler(iss);

	handler.push_context("game");

	int a = 0;
	int b = 0;

	handler.get_context("game")->register_function_button(
		"JUMP",
		[&](InputContext&) { a++; }
	);

	handler.get_context("game")->register_function_button(
		"MENU",
		[&](InputContext&) { b++; }
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
	REQUIRE(a == b);
}

TEST_CASE("increment_simple_mapped", "[input][inputhandler]") {
	std::istringstream iss(simple_file);

	InputHandler handler(iss);

	handler.push_context("game");

	int a = 0;

	handler.get_context("game")->register_function_button(
		"JUMP",
		[&](InputContext&) { a++; }
	);

	//press button 5 times
	InputEvent event = InputEvent::create_button(
		RawInputConstants::input::GAMEPAD_BUTTON_A,
		true
	);

	for (size_t i = 0; i < 5; ++i) {
		handler.handle_event(event);
		handler.execute();
	}

	REQUIRE(a == 5);
}