#pragma once

#include "enum.h"

#include <SDL2/SDL.h>
#include <imgui_impl_sdl_gl3.h>

#include <map>


/*
* Enums for every possible rawinput
*/

namespace eversim { namespace core { namespace input {

	namespace RawInputConstants
	{
		//merge button and range
		BETTER_ENUM(input, uint8_t,
			GAMEPAD_BUTTON_A = 0,
			GAMEPAD_BUTTON_B,
			GAMEPAD_BUTTON_X,
			GAMEPAD_BUTTON_Y,
			GAMEPAD_BUTTON_LB,
			GAMEPAD_BUTTON_RB,
			GAMEPAD_BUTTON_START,
			GAMEPAD_BUTTON_SELECT,
			GAMEPAD_BUTTON_DPAD_LEFT,
			GAMEPAD_BUTTON_DPAD_RIGHT,
			GAMEPAD_BUTTON_DPAD_UP,
			GAMEPAD_BUTTON_DPAD_DOWN,
			GAMEPAD_ANALOGUE_LEFT_STICK_X,
			GAMEPAD_ANALOGUE_LEFT_STICK_Y,
			GAMEPAD_ANALOGUE_RIGHT_STICK_X,
			GAMEPAD_ANALOGUE_RIGHT_STICK_Y,
			GAMEPAD_ANALOGUE_LT,
			GAMEPAD_ANALOGUE_RT,
			MOUSE_X,
			MOUSE_Y,
			MOUSE_BUTTON_LEFT,
			MOUSE_BUTTON_MIDDLE,
			MOUSE_BUTTON_RIGHT,
			MOUSE_BUTTON_LEFT_X,
			MOUSE_BUTTON_MIDDLE_X,
			MOUSE_BUTTON_RIGHT_X,
			MOUSE_BUTTON_LEFT_Y,
			MOUSE_BUTTON_MIDDLE_Y,
			MOUSE_BUTTON_RIGHT_Y,
			INVALID
		)

		BETTER_ENUM(event_type, uint8_t,
			BUTTON_DOWN = 0,
			BUTTON_UP,
			RANGE,
			INVALID
		)

		const std::map<input, std::pair<input,input>> mouse_remap =
		{
			{ input::MOUSE_BUTTON_LEFT,{ input::MOUSE_BUTTON_LEFT_X ,input::MOUSE_BUTTON_LEFT_Y } },
			{ input::MOUSE_BUTTON_MIDDLE,{ input::MOUSE_BUTTON_MIDDLE_X ,input::MOUSE_BUTTON_MIDDLE_Y } },
			{ input::MOUSE_BUTTON_RIGHT,{ input::MOUSE_BUTTON_RIGHT_X ,input::MOUSE_BUTTON_RIGHT_Y } },
		};

		const std::map<SDL_EventType,event_type> sdl_event_map =
		{
			{ SDL_JOYBUTTONDOWN , event_type::BUTTON_DOWN },
			{ SDL_CONTROLLERBUTTONDOWN , event_type::BUTTON_DOWN },
			{ SDL_JOYBUTTONUP , event_type::BUTTON_UP },
			{ SDL_CONTROLLERBUTTONUP , event_type::BUTTON_UP },
			{ SDL_JOYAXISMOTION , event_type::RANGE },
			{ SDL_MOUSEMOTION, event_type::RANGE },
			{ SDL_MOUSEBUTTONDOWN, event_type::RANGE },
			{ SDL_MOUSEBUTTONUP, event_type::RANGE }
		};

		const std::map<SDL_GameControllerButton, input> sdl_button_map =
		{
			{SDL_CONTROLLER_BUTTON_A,input::GAMEPAD_BUTTON_A},
			{SDL_CONTROLLER_BUTTON_B,input::GAMEPAD_BUTTON_B},
			{SDL_CONTROLLER_BUTTON_X,input::GAMEPAD_BUTTON_X},
			{SDL_CONTROLLER_BUTTON_Y,input::GAMEPAD_BUTTON_Y},
			{SDL_CONTROLLER_BUTTON_BACK,input::GAMEPAD_BUTTON_LB},
			{SDL_CONTROLLER_BUTTON_GUIDE,input::GAMEPAD_BUTTON_RB},
			{SDL_CONTROLLER_BUTTON_LEFTSTICK,input::GAMEPAD_BUTTON_START},
			{SDL_CONTROLLER_BUTTON_START,input::GAMEPAD_BUTTON_SELECT}
		};

		const std::map<uint8_t, input> sdl_mouserange_map =
		{
			{SDL_BUTTON_LEFT, input::MOUSE_BUTTON_LEFT},
			{SDL_BUTTON_MIDDLE, input::MOUSE_BUTTON_MIDDLE},
			{SDL_BUTTON_RIGHT, input::MOUSE_BUTTON_RIGHT}
		};

		const std::map<SDL_GameControllerAxis, input> sdl_range_map =
		{
			{ SDL_CONTROLLER_AXIS_LEFTX ,input::GAMEPAD_ANALOGUE_LEFT_STICK_X },
			{ SDL_CONTROLLER_AXIS_LEFTY ,input::GAMEPAD_ANALOGUE_LEFT_STICK_Y },
			{ SDL_CONTROLLER_AXIS_RIGHTX ,input::GAMEPAD_ANALOGUE_LT },
			{ SDL_CONTROLLER_AXIS_RIGHTY ,input::GAMEPAD_ANALOGUE_RIGHT_STICK_X },
			{ SDL_CONTROLLER_AXIS_TRIGGERLEFT ,input::GAMEPAD_ANALOGUE_RIGHT_STICK_Y },
			{ SDL_CONTROLLER_AXIS_TRIGGERRIGHT ,input::GAMEPAD_ANALOGUE_RT }
		};
	}

	
}}}