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
			GAMEPAD_ANALOGUE_LEFT_STICK,
			GAMEPAD_ANALOGUE_RIGHT_STICK,
			GAMEPAD_ANALOGUE_LEFT_STICK_X,
			GAMEPAD_ANALOGUE_LEFT_STICK_Y,
			GAMEPAD_ANALOGUE_RIGHT_STICK_X,
			GAMEPAD_ANALOGUE_RIGHT_STICK_Y,
			GAMEPAD_ANALOGUE_LT,
			GAMEPAD_ANALOGUE_RT,
			MOUSE,
			MOUSE_BUTTON_LEFT,
			MOUSE_BUTTON_MIDDLE,
			MOUSE_BUTTON_RIGHT,
			KEY_ESC,
			KEY_F1,
			KEY_F2,
			KEY_F3,
			KEY_F4,
			KEY_F5,
			KEY_F6,
			KEY_F7,
			KEY_F8,
			KEY_F9,
			KEY_F10,
			KEY_F11,
			KEY_F12,
			KEY_0,
			KEY_1,
			KEY_2,
			KEY_3,
			KEY_4,
			KEY_5,
			KEY_6,
			KEY_7,
			KEY_8,
			KEY_9,
			KEY_A,
			KEY_B,
			KEY_C,		
			KEY_D,
			KEY_E,
			KEY_F,
			KEY_G,
			KEY_H,
			KEY_I,
			KEY_J,
			KEY_K,
			KEY_L,
			KEY_M,
			KEY_N,
			KEY_O,
			KEY_P,
			KEY_Q,
			KEY_R,
			KEY_S,
			KEY_T,
			KEY_U,
			KEY_V,
			KEY_W,
			KEY_X,
			KEY_Y,
			KEY_Z,
			KEY_BACKSPACE,
			KEY_SPACE,
			KEY_ENTER,
			KEY_LEFT_SHIFT,
			KEY_RIGHT_SHIFT,
			KEY_LEFT_CTRL,
			KEY_RIGHT_CTRL,
			KEY_CAPSLOCK,
			KEY_TAB,
			KEY_LEFT_ALT,
			KEY_RIGHT_ALT,
			INVALID
		)

		BETTER_ENUM(event_type, uint8_t,
			BUTTON_DOWN = 0,
			BUTTON_UP,
			RANGE,
			INVALID
		)

		const std::map<SDL_EventType,event_type> sdl_event_map =
		{
			{ SDL_JOYBUTTONDOWN , event_type::BUTTON_DOWN },
			{ SDL_CONTROLLERBUTTONDOWN , event_type::BUTTON_DOWN },
			{ SDL_JOYBUTTONUP , event_type::BUTTON_UP },
			{ SDL_CONTROLLERBUTTONUP , event_type::BUTTON_UP },
			{ SDL_JOYAXISMOTION , event_type::RANGE },
			{ SDL_MOUSEMOTION, event_type::RANGE },
			{ SDL_MOUSEBUTTONDOWN, event_type::RANGE },
			{ SDL_MOUSEBUTTONUP, event_type::RANGE },
			{ SDL_KEYDOWN , event_type::BUTTON_DOWN},
			{ SDL_KEYUP , event_type::BUTTON_UP }
		};

		const std::map<SDL_GameControllerButton, input> sdl_button_map =
		{
			{ SDL_CONTROLLER_BUTTON_A,input::GAMEPAD_BUTTON_A},
			{ SDL_CONTROLLER_BUTTON_B,input::GAMEPAD_BUTTON_B},
			{ SDL_CONTROLLER_BUTTON_X,input::GAMEPAD_BUTTON_X},
			{ SDL_CONTROLLER_BUTTON_Y,input::GAMEPAD_BUTTON_Y},
			{ SDL_CONTROLLER_BUTTON_BACK,input::GAMEPAD_BUTTON_LB},
			{ SDL_CONTROLLER_BUTTON_GUIDE,input::GAMEPAD_BUTTON_RB},
			{ SDL_CONTROLLER_BUTTON_LEFTSTICK,input::GAMEPAD_BUTTON_START},
			{ SDL_CONTROLLER_BUTTON_START,input::GAMEPAD_BUTTON_SELECT}
		};

		const std::map<SDL_Keycode, input> sdl_keycode_map =
		{
			{ SDLK_ESCAPE, input::KEY_ESC},
			{ SDLK_LSHIFT, input::KEY_LEFT_SHIFT },
			{ SDLK_RSHIFT, input::KEY_RIGHT_SHIFT },
			{ SDLK_LCTRL, input::KEY_LEFT_CTRL },
			{ SDLK_RCTRL, input::KEY_RIGHT_CTRL },
			{ SDLK_CAPSLOCK, input::KEY_CAPSLOCK },
			{ SDLK_TAB , input::KEY_TAB },
			{ SDLK_LALT, input::KEY_LEFT_ALT },
			{ SDLK_RALT , input::KEY_RIGHT_ALT },
			{ SDLK_BACKSPACE, input::KEY_BACKSPACE },
			{ SDLK_SPACE, input::KEY_SPACE},
			{ SDLK_F1, input::KEY_F1 },
			{ SDLK_F1, input::KEY_F2 },
			{ SDLK_F2, input::KEY_F3 },
			{ SDLK_F3, input::KEY_F4 },
			{ SDLK_F4, input::KEY_F5 },
			{ SDLK_F5, input::KEY_F6 },
			{ SDLK_F6, input::KEY_F7 },
			{ SDLK_F7, input::KEY_F8 },
			{ SDLK_F9, input::KEY_F9 },
			{ SDLK_F10, input::KEY_F10 },
			{ SDLK_F11, input::KEY_F11 },
			{ SDLK_F12, input::KEY_F12 }, 
			{ SDLK_0 , input::KEY_0},
			{ SDLK_1 , input::KEY_1 },
			{ SDLK_2 , input::KEY_2 },
			{ SDLK_3 , input::KEY_3 },
			{ SDLK_4 , input::KEY_4 },
			{ SDLK_5 , input::KEY_5 },
			{ SDLK_6 , input::KEY_6 },
			{ SDLK_7 , input::KEY_7 },
			{ SDLK_8 , input::KEY_8 },
			{ SDLK_9 , input::KEY_9 },
			{ SDLK_a , input::KEY_A},
			{ SDLK_b , input::KEY_B },
			{ SDLK_c , input::KEY_C },
			{ SDLK_d , input::KEY_D },
			{ SDLK_e , input::KEY_E },
			{ SDLK_f , input::KEY_F },
			{ SDLK_g , input::KEY_G },
			{ SDLK_h , input::KEY_H },
			{ SDLK_i , input::KEY_I },
			{ SDLK_j , input::KEY_J },
			{ SDLK_k , input::KEY_K },
			{ SDLK_l , input::KEY_L },
			{ SDLK_m , input::KEY_M },
			{ SDLK_n , input::KEY_N },
			{ SDLK_o , input::KEY_O },
			{ SDLK_p , input::KEY_P },
			{ SDLK_q , input::KEY_Q },
			{ SDLK_r , input::KEY_R },
			{ SDLK_s , input::KEY_S },
			{ SDLK_t , input::KEY_T },
			{ SDLK_u , input::KEY_U },
			{ SDLK_v , input::KEY_V },
			{ SDLK_w , input::KEY_W },
			{ SDLK_x , input::KEY_X },
			{ SDLK_y , input::KEY_Y },
			{ SDLK_z , input::KEY_Z }
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