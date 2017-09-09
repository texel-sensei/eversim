#pragma once

#include "core/reflection/static_member.h"

#include <boost/mpl/range_c.hpp>

#include <boost/fusion/include/for_each.hpp>
#include <boost/fusion/include/zip.hpp>
#include <boost/fusion/include/at_c.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/mpl.hpp>
#include <boost/fusion/include/is_sequence.hpp>


namespace eversim::core::reflection
{

	template<typename T>
	using require_fusion_sequence = std::enable_if_t<boost::fusion::traits::is_sequence<std::decay_t<T>>::value>;

	template<size_t N, typename Object,
		typename = require_fusion_sequence<Object>
	>
		auto get_member_by_index(Object&& object)
	{
		namespace fuz = boost::fusion;
		namespace mpl = boost::mpl;

		using bare_type = std::decay_t<Object>;
		using name = fuz::extension::struct_member_name<bare_type, N>;

		return make_member(
			fuz::at_c<N>(object),
			name::call()
		);
	}

	template<typename T, typename Func,
		typename = require_fusion_sequence<T>
	>
		void iterate_members(T&& object, Func&& f)
	{
		namespace fuz = boost::fusion;
		namespace mpl = boost::mpl;

		using bare_type = std::decay_t<T>;

		fuz::for_each(mpl::range_c<unsigned, 0, fuz::result_of::size<bare_type>::value>(),
			[&](auto idx)
		{
			f(
				get_member_by_index<idx>(object)
			);
		}
		);
	}

} // namespace