#include "BeatmapController.hpp"

void BeatmapController::setDifficulties(std::vector<int> beatmaps_v)
{
	beatmaps b_table{};
	himitsu::Connection db = himitsu::ConnectionPool::createConnection();
	for (const int id : beatmaps_v)
	{
		auto [unused1, std_output]   = himitsu::curl::get("https://old.ppy.sh/api/get_beatmaps?k=" + config::api_key + "a=1&m=0&b=" + std::to_string(id));
		auto [unused2, taiko_output] = himitsu::curl::get("https://old.ppy.sh/api/get_beatmaps?k=" + config::api_key + "a=1&m=1&b=" + std::to_string(id));
		auto [unused3, ctb_output]   = himitsu::curl::get("https://old.ppy.sh/api/get_beatmaps?k=" + config::api_key + "a=1&m=2&b=" + std::to_string(id));
		auto [unused4, mania_output] = himitsu::curl::get("https://old.ppy.sh/api/get_beatmaps?k=" + config::api_key + "a=1&m=3&b=" + std::to_string(id));

		json jsonRoot;

		jsonRoot = json::parse(std_output, nullptr, false);
		if (!jsonRoot.is_discarded())
		{
			if (!jsonRoot[0]["difficultyRating"].is_null())
				(*db)(sqlpp::update(b_table)
					.set(b_table.difficulty_std = jsonRoot[0]["difficultyRating"].get<float>())
					.where(b_table.beatmap_id == id));
		}

		jsonRoot = json::parse(taiko_output, nullptr, false);
		if (!jsonRoot.is_discarded())
		{
			if (!jsonRoot[0]["difficultyRating"].is_null())
				(*db)(sqlpp::update(b_table)
					.set(b_table.difficulty_taiko = jsonRoot[0]["difficultyRating"].get<float>())
					.where(b_table.beatmap_id == id));
		}

		jsonRoot = json::parse(ctb_output, nullptr, false);
		if (!jsonRoot.is_discarded())
		{
			if (!jsonRoot[0]["difficultyRating"].is_null())
				(*db)(sqlpp::update(b_table)
					.set(b_table.difficulty_ctb = jsonRoot[0]["difficultyRating"].get<float>())
					.where(b_table.beatmap_id == id));
		}

		jsonRoot = json::parse(mania_output, nullptr, false);
		if (!jsonRoot.is_discarded())
		{
			if (!jsonRoot[0]["difficultyRating"].is_null())
				(*db)(sqlpp::update(b_table)
					.set(b_table.difficulty_mania = jsonRoot[0]["difficultyRating"].get<float>())
					.where(b_table.beatmap_id == id));
		}

		// Don't overuse osu!API, always remember about 60 request per minute
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}