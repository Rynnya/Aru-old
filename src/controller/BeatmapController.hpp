#ifndef BeatmapController_hpp
#define BeatmapController_hpp

#include "Globals.hpp"

#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"

#include "database/tables/BeatmapTable.hpp"
#include "database/tables/ScoresTable.hpp"
#include "database/tables/UsersTable.hpp"

#include <thread>

#include OATPP_CODEGEN_BEGIN(ApiController)

class BeatmapController : public oatpp::web::server::api::ApiController {
private:
	typedef BeatmapController __ControllerType;
public:
	BeatmapController(OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper))
		: oatpp::web::server::api::ApiController(objectMapper)
	{}
	void setDifficulties(std::vector<int> beatmaps_v);
public:

	// Perform download from osu!API, requested by Bancho
	ENDPOINT("POST", "/beatmapset/{id}/download", beatmapsDownload, PATH(Int32, id))
	{
		if (!config::api_enabled || config::api_key == "")
		{
			return createResponse(Status::CODE_410,
				himitsu::createError(Status::CODE_410, "API disabled").c_str()
			);
		}

		std::string main_output = "";
		auto [success, std_output] = himitsu::curl::get("https://old.ppy.sh/api/get_beatmaps?k=" + config::api_key + "&s=" + std::to_string(id));
		if (!success)
			return createResponse(Status::CODE_400,
				himitsu::createError(Status::CODE_400, "Cannot get data from osu!API").c_str()
			);

		json jsonRoot = json::parse(std_output, nullptr, false);
		if (!jsonRoot.is_discarded())
		{
			if (!jsonRoot["error"].is_null())
			{
				config::api_enabled = false;
				return createResponse(Status::CODE_401,
					himitsu::createError(Status::CODE_401, "Wrong API key").c_str()
				);
			}
			beatmaps b_table{};
			std::vector<int> b_maps;
			auto db = himitsu::ConnectionPool::getInstance()->getConnection();
			for (const auto& beatmap : jsonRoot)
			{
				int mode = beatmap["mode"];
				int beatmap_id = beatmap["beatmap_id"];
				(**db)(sqlpp::insert_into(b_table).set(
					b_table.beatmap_id = beatmap_id,
					b_table.beatmapset_id = beatmap["beatmapset_id"].get<int>(),
					b_table.beatmap_md5 = beatmap["file_md5"].get<std::string>(),
					b_table.artist = beatmap["artist"].get<std::string>(),
					b_table.title = beatmap["title"].get<std::string>(),
					b_table.difficulty_name = beatmap["version"].get<std::string>(),
					b_table.creator = beatmap["creator"].get<std::string>(),
					b_table.file_name = fmt::format("{} - {} ({}) [{}].osu", 
						beatmap["artist"].get<std::string>(),
						beatmap["title"].get<std::string>(),
						beatmap["creator"].get<std::string>(),
						beatmap["version"].get<std::string>()
					),
					b_table.cs = beatmap["diff_size"].get<float>(),
					b_table.ar = beatmap["diff_approach"].get<float>(),
					b_table.od = beatmap["diff_overall"].get<float>(),
					b_table.hp = beatmap["diff_drain"].get<float>(),
					b_table.mode = mode,
					b_table.difficulty_std = 0,
					b_table.difficulty_taiko = 0,
					b_table.difficulty_ctb = 0,
					b_table.difficulty_mania = 0,
					b_table.max_combo = beatmap["max_combo"].is_null() ? 0 : beatmap["max_combo"].get<int>(),
					b_table.hit_length = beatmap["hit_length"].get<int>(),
					b_table.bpm = beatmap["bpm"].get<int>(),
					b_table.count_normal = beatmap["count_normal"].get<int>(),
					b_table.count_slider = beatmap["count_slider"].get<int>(),
					b_table.count_spinner = beatmap["count_spinner"].get<int>(),
					b_table.ranked = beatmap["approved"] <= 0 ? 0 : beatmap["approved"].get<int>() + 1,
					b_table.latest_update = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count(),
					b_table.creating_date = himitsu::time_convert::getTimestamp(beatmap["submit_date"].get<std::string>())
				));
				if (mode != 0) // Non-convertable
				{
					switch (mode)
					{
						case 1:
							(**db)(sqlpp::update(b_table)
								.set(b_table.difficulty_taiko = beatmap["difficulty_rating"].get<float>())
								.where(b_table.beatmap_id == beatmap_id));
							break;
						case 2:
							(**db)(sqlpp::update(b_table)
								.set(b_table.difficulty_ctb = beatmap["difficulty_rating"].get<float>())
								.where(b_table.beatmap_id == beatmap_id));
							break;
						case 3:
							(**db)(sqlpp::update(b_table)
								.set(b_table.difficulty_mania = beatmap["difficulty_rating"].get<float>())
								.where(b_table.beatmap_id == beatmap_id));
							break;
						default: // how?
							break;
					}
					// Don't put map into download pool
					continue;
				}
				b_maps.push_back(beatmap_id);
			}
			
			if (!b_maps.empty())
			{
				std::thread diffs(&BeatmapController::setDifficulties, *this, b_maps);
				diffs.detach();
			}

			auto response = createResponse(Status::CODE_200, "OK");
			response->putHeader("Content-Type", "text/plain");
			return response;
		}

		return createResponse(Status::CODE_400,
			himitsu::createError(Status::CODE_400, "Cannot get data from osu!API").c_str()
		);
	};

	ENDPOINT("GET", "/beatmapset/{id}", beatmapSet, PATH(Int32, id))
	{
		beatmaps b_table{};
		auto db = himitsu::ConnectionPool::getInstance()->getConnection();
		auto result = (**db)(sqlpp::select(b_table.beatmap_id, b_table.beatmapset_id, b_table.artist, b_table.title, b_table.difficulty_name,
			b_table.creator, b_table.count_normal, b_table.count_slider, b_table.count_spinner, b_table.max_combo, b_table.ranked,
			b_table.creating_date, b_table.difficulty_std, b_table.difficulty_taiko, b_table.difficulty_ctb, b_table.difficulty_mania,
			b_table.bpm, b_table.hit_length, b_table.cs, b_table.ar, b_table.od, b_table.hp, b_table.mode
		).from(b_table).where(b_table.beatmapset_id == (*id)));

		if (result.empty())
		{
			return createResponse(Status::CODE_404,
				himitsu::createError(Status::CODE_404, "cannot find beatmap set").c_str()
			);
		}

		json response = json::array();
		for (const auto& row : result)
		{
			json beatmap;
			beatmap["beatmap_id"]    = row.beatmap_id.value();
			beatmap["beatmapset_id"] = row.beatmapset_id.value();
			beatmap["artist"]        = row.artist.value();
			beatmap["title"]         = row.title.value();
			beatmap["version"]       = row.difficulty_name.value();
			beatmap["creator"]       = row.creator.value();
			beatmap["count_normal"]  = row.count_normal.value();
			beatmap["count_slider"]  = row.count_slider.value();
			beatmap["count_spinner"] = row.count_spinner.value();
			beatmap["max_combo"]     = row.max_combo.value();
			beatmap["ranked"]        = row.ranked.value();
			beatmap["creating_date"] = row.creating_date.value();
			beatmap["bpm"]           = row.bpm.value();
			beatmap["hit_length"]    = row.hit_length.value();

			switch (row.mode)
			{
				default:
					beatmap["difficulty"] = row.difficulty_std.value();
					break;
				case 1:
					beatmap["difficulty"] = row.difficulty_taiko.value();
					break;
				case 2:
					beatmap["difficulty"] = row.difficulty_ctb.value();
					break;
				case 3:
					beatmap["difficulty"] = row.difficulty_mania.value();
					break;
			}

			json diff;
			diff["std"]   = row.difficulty_std.value();
			diff["taiko"] = row.difficulty_taiko.value();
			diff["ctb"]   = row.difficulty_ctb.value();
			diff["mania"] = row.difficulty_mania.value();
			beatmap["difficulties"] = diff;

			beatmap["cs"] = row.cs.value();
			beatmap["ar"] = row.ar.value();
			beatmap["od"] = row.od.value();
			beatmap["hp"] = row.hp.value();

			beatmap["mode"] = row.mode.value();
			response.push_back(beatmap);
		}

		return createResponse(Status::CODE_200, response.dump().c_str());
	};

	ENDPOINT("GET", "/beatmap/{id}", beatmapDefault, PATH(Int32, id))
	{
		beatmaps b_table{};
		auto db = himitsu::ConnectionPool::getInstance()->getConnection();
		auto result = (**db)(sqlpp::select(b_table.beatmap_id, b_table.beatmapset_id, b_table.artist, b_table.title, b_table.difficulty_name,
			b_table.creator, b_table.count_normal, b_table.count_slider, b_table.count_spinner, b_table.max_combo, b_table.ranked,
			b_table.creating_date, b_table.difficulty_std, b_table.difficulty_taiko, b_table.difficulty_ctb, b_table.difficulty_mania,
			b_table.bpm, b_table.hit_length, b_table.cs, b_table.ar, b_table.od, b_table.hp, b_table.mode
		).from(b_table).where(b_table.beatmap_id == (*id)).limit(1u));

		if (result.empty())
		{
			return createResponse(Status::CODE_404,
				himitsu::createError(Status::CODE_404, "cannot find beatmap").c_str()
			);
		}

		const auto& row = result.front();
		json beatmap;

		beatmap["beatmap_id"] = row.beatmap_id.value();
		beatmap["beatmapset_id"] = row.beatmapset_id.value();
		beatmap["artist"] = row.artist.value();
		beatmap["title"] = row.title.value();
		beatmap["version"] = row.difficulty_name.value();
		beatmap["creator"] = row.creator.value();
		beatmap["count_normal"] = row.count_normal.value();
		beatmap["count_slider"] = row.count_slider.value();
		beatmap["count_spinner"] = row.count_spinner.value();
		beatmap["max_combo"] = row.max_combo.value();
		beatmap["ranked"] = row.ranked.value();
		beatmap["creating_date"] = row.creating_date.value();
		beatmap["bpm"] = row.bpm.value();
		beatmap["hit_length"] = row.hit_length.value();

		switch (row.mode)
		{
			default:
				beatmap["difficulty"] = row.difficulty_std.value();
				break;
			case 1:
				beatmap["difficulty"] = row.difficulty_taiko.value();
				break;
			case 2:
				beatmap["difficulty"] = row.difficulty_ctb.value();
				break;
			case 3:
				beatmap["difficulty"] = row.difficulty_mania.value();
				break;
		}

		json diff;
		diff["std"] = row.difficulty_std.value();
		diff["taiko"] = row.difficulty_taiko.value();
		diff["ctb"] = row.difficulty_ctb.value();
		diff["mania"] = row.difficulty_mania.value();
		beatmap["difficulties"] = diff;

		beatmap["cs"] = row.cs.value();
		beatmap["ar"] = row.ar.value();
		beatmap["od"] = row.od.value();
		beatmap["hp"] = row.hp.value();

		beatmap["mode"] = row.mode.value();

		return createResponse(Status::CODE_200, beatmap.dump().c_str());
	};

	ENDPOINT("GET", "/beatmap/{id}/leaderboard", beatmapLeaderboard, 
		PATH(Int32, id), QUERY(Int32, mode, "mode", "-1"), QUERY(Int32, relax, "relax", "0"), QUERY(Int32, length, "length", "50"))
	{
		beatmaps b_table{};
		auto db = himitsu::ConnectionPool::getInstance()->getConnection();

		int _length = SQLHelper::Limitize(1, length, 100);
		bool isRelax = himitsu::utils::intToBoolean(relax);
		int play_mode = mode;
		if (play_mode == 3 && isRelax)
		{
			return createResponse(Status::CODE_404,
				himitsu::createError(Status::CODE_404, "mania don't have relax mode").c_str()
			);
		}

		std::string md5 = "";
		auto res = (**db)(sqlpp::select(b_table.mode, b_table.beatmap_md5)
			.from(b_table)
			.where(b_table.beatmap_id == (*id))
			.limit(1u)
		);
		if (res.empty())
		{
			return createResponse(Status::CODE_404,
				himitsu::createError(Status::CODE_404, "cannot find beatmap").c_str()
			);
		}
		const auto& r = res.front();
		if (play_mode == -1) play_mode = r.mode;
		md5 = r.beatmap_md5;

		users u_table{};
		scores s_table{};

		auto query = sqlpp::select(s_table.userid, u_table.username, u_table.country,  s_table.score, s_table.pp, s_table.accuracy,
			s_table.count_300, s_table.count_100, s_table.count_50, s_table.count_misses, s_table.max_combo, s_table.mods)
			.from(s_table.join(u_table).on(s_table.userid == u_table.id))
			.where(s_table.beatmap_md5 == md5 and s_table.is_relax == isRelax and s_table.play_mode == play_mode)
			.order_by(s_table.pp.desc());
		std::pair<unsigned int, unsigned int> limit = SQLHelper::Paginate(1, _length, 100);
		auto result = (**db)(query.offset(limit.first).limit(limit.second));

		json response = json::array();
		for (const auto& row : result)
		{
			json score;
			score["user_id"]      = row.userid.value();
			score["username"]     = row.username.value();
			score["country"]      = row.country.value();
			score["score"]        = row.score.value();
			score["pp"]           = row.pp.value();
			score["accuracy"]     = row.accuracy.value();
			score["count_300"]    = row.count_300.value();
			score["count_100"]    = row.count_100.value();
			score["count_50"]     = row.count_50.value();
			score["count_miss"]   = row.count_misses.value();
			score["max_combo"]    = row.max_combo.value();
			score["mods"]         = row.mods.value();
			response.push_back(score);
		}

		return createResponse(Status::CODE_200, response.dump().c_str());
	};

};

#include OATPP_CODEGEN_END(ApiController)

#endif