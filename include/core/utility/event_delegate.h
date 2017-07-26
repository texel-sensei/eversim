#pragma once

#include <vector>
#include <algorithm>
#include <functional>
#include <memory>

// Modified version of
// http://www.gamedev.net/page/resources/_/technical/general-programming/using-varadic-templates-for-a-signals-and-slots-implementation-in-c-r3782

namespace eversim {namespace core {namespace utility {

	template <class... Args>
	class Event;

	class BaseDelegate {
	public:
		virtual ~BaseDelegate()
		{
		}
	};

	template <class... Args>
	class AbstractDelegate : public BaseDelegate {
	protected:
		virtual ~AbstractDelegate()
		{
			for (auto i : v)
				i->disconnect(*this);
		}

		friend class Event<Args...>;

		void add(Event<Args...>* s) { v.push_back(s); }
		void remove(Event<Args...>* s) { v.erase(std::remove(v.begin(), v.end(), s), v.end()); }

		virtual void call(Args ... args) = 0;

		std::vector<Event<Args...>*> v;
	};

	template <class... Args>
	class ConcreteDelegate : public AbstractDelegate<Args...> {
	public:
		ConcreteDelegate(std::function<void(Args ...)> f, Event<Args...>& s): f(f)
		{
			s.connect(*this);
		}

		ConcreteDelegate(const ConcreteDelegate&) = delete;
		void operator=(const ConcreteDelegate&) = delete;
	private:

		friend class Event<Args...>;

		void call(Args ... args) override { f(args...); }
		std::function<void(Args ...)> f;
	};

	template <class... Args>
	class Event {
	public:
		Event()
		{
		}

		~Event()
		{
			for (auto i: v)
				i->remove(this);
		}

		void connect(AbstractDelegate<Args...>& s)
		{
			v.push_back(&s);
			s.add(this);
		}

		void disconnect(AbstractDelegate<Args...>& s)
		{
			v.erase(std::remove(v.begin(), v.end(), &s), v.end());
		}

		void operator()(Args ... args)
		{
			for (auto i: v)
				i->call(args...);
		}

	private:
		Event(const Event&) = delete;
		void operator=(const Event&) = delete;

		std::vector<AbstractDelegate<Args...>*> v;
	};

	class Delegate {
	public:
		Delegate()
		{
		}

		template <typename Func, class... Args>
		void connect(Func&& f, Event<Args...>& s)
		{
			v.push_back(
				std::make_unique<ConcreteDelegate<Args...>>(
					std::function<void(Args ...)>(std::forward<Func>(f)), s)
			);
		}

	private:
		Delegate(const Delegate&) = delete;
		void operator=(const Delegate&) = delete;

		std::vector<std::unique_ptr<BaseDelegate>> v;
	};

}}}
