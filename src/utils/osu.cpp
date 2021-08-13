#include "osu.hpp"

int32_t aru::osu_level::get_level(uint64_t score)
{
	if (score >= score_to_level[99])
	{
		return 100 + static_cast<int32_t>((score - score_to_level[99]) / 100000000000);
	}

	for (int32_t i = 0; i != 100; i++)
	{
		if (score_to_level[i] > score)
		{
			return i;
		}
	}

	return 1;
}

uint64_t aru::osu_level::get_required_score_for_level(int32_t level)
{
	if (level <= 0)
	{
		return 0;
	}

	if (level <= 100)
	{
		return score_to_level[level - 1];
	}

	return score_to_level[99] + 100000000000 * (static_cast<int64_t>(level) - 100);
}

double aru::osu_level::get_level_precise(uint64_t score)
{
	int32_t base_level = get_level(score);
	int64_t base_level_score = get_required_score_for_level(base_level);
	int64_t score_progress = score - base_level_score;
	int64_t score_level_diff = get_required_score_for_level(base_level + 1) - base_level_score;

	double result = double(score_progress) / double(score_level_diff) + double(base_level);
	return aru::math::is_infinite(result) || aru::math::is_nan(result) ? 0 : result;
}

std::string aru::osu::mode_to_string(int32_t number)
{
	switch (number)
	{
		default:
		case 0:
			return "std";
		case 1:
			return "taiko";
		case 2:
			return "ctb";
		case 3:
			return "mania";
	}
}

int32_t aru::osu::mode_to_int(const std::string& mode)
{
	static const std::unordered_map<std::string, int32_t> modes =
	{
		{ "std",   0 },
		{ "taiko", 1 },
		{ "ctb",   2 },
		{ "mania", 3 }
	};
	auto it = modes.find(mode);
	return it == modes.end() ? 0 : it->second;
}

const std::vector<std::string> aru::memes =
{
	"693 never gone", // Noshiko's ban
	"https://www.youtube.com/watch?v=dQw4w9WgXcQ", // RickRoll
	"PP when?", // My laziness
	"rm -rf /" // :D
};

const uint64_t aru::score_to_level[100] =
{
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
		26931190829
};