#ifndef utils_convent_hpp_included
#define utils_convent_hpp_included

#include <oatpp/core/Types.hpp>

namespace aru
{
	class convert
	{
	private:
        inline static int32_t safe_int(const char* ptr)
        {
            char* end_ptr;
            const long ans = std::strtol(ptr, &end_ptr, 10);

            if (ptr == end_ptr)
            {
                return -1;
            }

            if (ans == INT_MAX)
            {
                return -1; // I don't think someone can acquire this ID, so we safe :)
            }

            return ans;
        }
    public:
        inline static int32_t safe_int(const oatpp::String& str, int32_t def_value = -1)
        { 
            return str == nullptr ? def_value : safe_int(str->c_str());
        }
	};
}

#endif