#pragma once

#include <boost/mpl/range_c.hpp>
#include <boost/fusion/include/for_each.hpp>
#include <boost/fusion/include/zip.hpp>
#include <boost/fusion/include/at_c.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/mpl.hpp>
#include <boost/type_index.hpp>

#include <type_traits>


namespace eversim {namespace core{namespace utility {

	template<typename T>
	struct Type {
		using type = T;

		constexpr Type() = default;
		Type& operator=(const Type&) = delete;

		constexpr auto name() const
		{
			return boost::typeindex::type_id_with_cvr<type>().pretty_name();
		}
	};

	namespace detail {
		template<typename Val>
		struct member {
			Val value;
			const char* name;
			Type<std::remove_reference_t<Val>> type;
			Type<Val> full_type;

			member(Val value, const char* name) : value(value), name(name) {}
		};

		template<typename T>
		decltype(auto) make_member(T&& t, const char* name)
		{
			return member<T>(std::forward<T>(t), name);
		}
	} /* namespace detail */

	template<typename T, typename Func>
	void iterate_members(T&& object, Func&& f)
	{
		namespace fuz = boost::fusion;
		namespace mpl = boost::mpl;

		using bare_type = std::decay_t<T>;

		fuz::for_each(mpl::range_c<unsigned, 0, fuz::result_of::size<bare_type>::value>(),
		[&](auto idx)
		{
			using name = fuz::extension::struct_member_name<bare_type, idx>;
			f(
				detail::make_member(
					fuz::at_c<idx>(object),
					name::call()
				)
			);
		}
		);
	}
} } }