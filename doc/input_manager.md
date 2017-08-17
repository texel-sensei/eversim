input.GetAxis("x");

No strings, we want a mapping from strings to enums 
input.GetKey("jump");

This needs a file with contents like
	gamepade_button_o "jump"

input_manager.register("jump",callback_func,on_hold | on_release | on_press);
input_manager.register_all(callback_func);

input_manger.set_context("menu");
input_manager.restore_context(); 

Set context adds the new context to the context stack and switches to it.
Restore context by deleting the context on top of the stack.