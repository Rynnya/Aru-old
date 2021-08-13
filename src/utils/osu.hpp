#ifndef utils_osu_hpp_included
#define utils_osu_hpp_included

#include "globals.hpp"

namespace aru
{
	extern const uint64_t score_to_level[100];
	extern const std::vector<std::string> memes;

	class osu_level
	{
	public:
		static int32_t get_level(uint64_t score);
		static uint64_t get_required_score_for_level(int32_t level);
		static double get_level_precise(uint64_t score);
	};

	class osu
	{
	public:
		static std::string mode_to_string(int32_t number);
		static int32_t mode_to_int(const std::string& mode);
	};
}

#endif
