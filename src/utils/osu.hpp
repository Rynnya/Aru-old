#ifndef osuHimitsu_hpp
#define osuHimitsu_hpp

#include "Globals.hpp"
#include <vector>

namespace himitsu
{
	static const long long sc[100] = {
		0,
		30000,
		130000,
		340000,
		700000,
		1250000,
		2030000,
		3080000,
		4440000,
		6150000,
		8250000,
		10780000,
		13780000,
		17290000,
		21350000,
		26000000,
		31280000,
		37230000,
		43890000,
		51300000,
		59500000,
		68530000,
		78430000,
		89240000,
		101000000,
		113750000,
		127530000,
		142380000,
		158340000,
		175450000,
		193750000,
		213280000,
		234080000,
		256190000,
		279650000,
		304500000,
		330780000,
		358530000,
		387790000,
		418600000,
		451000000,
		485030000,
		520730000,
		558140000,
		597300000,
		638250000,
		681030000,
		725680000,
		772240000,
		820750000,
		871250000,
		923780000,
		978380000,
		1035090000,
		1093950000,
		1155000000,
		1218280000,
		1283830000,
		1351690001,
		1421900001,
		1494500002,
		1569530004,
		1647030007,
		1727040013,
		1809600024,
		1894750043,
		1982530077,
		2072980138,
		2166140248,
		2262050446,
		2360750803,
		2462281446,
		2566682603,
		2673994685,
		2784258433,
		2897515180,
		3013807324,
		3133179183,
		3255678529,
		3381359353,
		3510286835,
		3642546304,
		3778259346,
		3917612824,
		4060911082,
		4208669948,
		4361785907,
		4521840633,
		4691649139,
		4876246450,
		5084663609,
		5333124496,
		5650800094,
		6090166168,
		6745647103,
		7787174786,
		9520594614,
		12496396305,
		17705429349,
		26931190829,
	};

	static std::vector<std::string> memes = { // Local old Himitsu memes
		"693 never gone", // Noshiko's ban
		"https://www.youtube.com/watch?v=dQw4w9WgXcQ", // RickRoll
		"PP when?", // My laziness
		"rm -rf /" // :D
	};

	class osu_level
	{
	public:
		static int GetLevel(long long score)
		{
			if (score <= 0) return 1;
			if (score >= sc[99]) return 100 + int((score - sc[99]) / 100000000000);
			for (int i = 0; i != 100; i++)
			{
				if (sc[i] > score)
					return i;
			}
			return 1;
		}

		static long long GetRequiredScoreForLevel(int level)
		{
			if (level <= 0) return 0;
			if (level <= 100) return sc[level - 1];
			return sc[99] + 100000000000 * (level - 100);
		}

		static double GetLevelPrecise(long long score)
		{
			int baseLevel = GetLevel(score);
			long long baseLevelScore = GetRequiredScoreForLevel(baseLevel);
			long long scoreProgress = score - baseLevelScore;
			long long scoreLevelDifference = GetRequiredScoreForLevel(baseLevel + 1) - baseLevelScore;

			double result = double(scoreProgress) / double(scoreLevelDifference) + double(baseLevel);
			if (himitsu::math::is_infinite(result) || himitsu::math::is_nan(result))
				return 0;
			return result;
		}
	};

	class osu
	{
	public:
		static std::string modeToString(int number)
		{
			switch (number)
			{
				default:
					return "std";
				case 1:
					return "taiko";
				case 2:
					return "ctb";
				case 3:
					return "mania";
			}
		}

		static int modeToInt(std::string mode)
		{
			if (mode == "mania") return 3;
			if (mode == "ctb")   return 2;
			if (mode == "taiko") return 1;
			return 0;
		}
	};
}

#endif