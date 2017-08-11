#pragma once
#include <memory>


namespace eversim {namespace core {namespace system {
	class gameobject;

	class component {
	public:
		/*
		 * Constructor of a gameobject. The first parameter has to be the pointer to the owner 
		 * gameobject. Else gameobject::add_component<>() does not work!
		 */
		explicit component(gameobject* owner) : owner(owner){}
		virtual ~component() = default;

		std::unique_ptr<component> clone() const;

	protected:
		virtual void update() = 0;
		virtual std::unique_ptr<component> do_clone() const = 0;
	private:
		gameobject* owner;
	};

} } }
