Game
	Game has
	Gameobject
		Gameobject has
		rotation, position, scale
		one or no parent object
		arbitrary number of children objects (normal ptr)
		has components (ptr ?)
		
		components
			physics
			graphics
		
			easy to talk from component to Gameobject
			but we also want crosstalk -> messages ?
			
			we want
				messages
				and (!)
				component getters

	
	Level
	Worldstate ?
		which quests
		money
	
	Gameloop 
		Statemachine, Pause
	