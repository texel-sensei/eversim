#include "core/rendering/texture_base_class.h"

namespace eversim
{
	namespace core
	{
		namespace rendering
		{
			size_t TextureBase::get_unique_id() const
			{
				return unique_id;
			}

			void TextureBase::set_unique_id(const size_t ui)
			{
				unique_id = ui;
			}

		}
	}
}
