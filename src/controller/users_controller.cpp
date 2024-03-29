#include "users_controller.hpp"

#include "database/tables/beatmap_table.hpp"
#include "database/tables/scores_table.hpp"

std::shared_ptr<users_controller::OutgoingResponse> users_controller::build_scores(
	const aru::database& db,
	int32_t id,
	int32_t user_mode,
	int32_t relax,
	int32_t page,
	int32_t length,
	scores_type type
) const
{
	aru::utils::sanitize(user_mode, 0, 3, get_mode(db, id));
	if (user_mode == -1)
	{
		auto error = aru::create_error(Status::CODE_404, "Player not found");
		return createResponse(Status::CODE_404, error);
	}

	if (relax == 1 && user_mode == 3)
	{
		auto error = aru::create_error(Status::CODE_400, "Mania don't have relax mode");
		return createResponse(Status::CODE_400, error);
	}

	length = std::clamp(length, 1, 100);
	json response = json::array();

	const tables::beatmaps beatmaps_table{};
	bool isRelax = aru::utils::int_to_bool(relax);

	const tables::scores scores_table{};
	const tables::users users_table{};
	auto query = sqlpp::dynamic_select(*db, scores_table.id, scores_table.ranking, scores_table.score, scores_table.full_combo, scores_table.mods,
		scores_table.count_300, scores_table.count_100, scores_table.count_50,
		scores_table.count_gekis, scores_table.count_katus, scores_table.count_misses,
		scores_table.time, scores_table.play_mode, scores_table.accuracy, scores_table.pp,
		scores_table.max_combo.as(score_t),

		beatmaps_table.beatmap_id, beatmaps_table.beatmap_md5, beatmaps_table.max_combo.as(beatmap_t),
		beatmaps_table.beatmapset_id, beatmaps_table.artist, beatmaps_table.title, beatmaps_table.difficulty_name,
		beatmaps_table.cs, beatmaps_table.ar, beatmaps_table.od, beatmaps_table.hp,
		beatmaps_table.difficulty_std, beatmaps_table.difficulty_taiko, beatmaps_table.difficulty_ctb, beatmaps_table.difficulty_mania,
		beatmaps_table.hit_length, beatmaps_table.ranked_status, beatmaps_table.ranked_status_freezed, beatmaps_table.latest_update
	).dynamic_from(scores_table.join(beatmaps_table).on(scores_table.beatmap_md5 == beatmaps_table.beatmap_md5)).dynamic_where(scores_table.is_relax == isRelax and scores_table.completed).dynamic_order_by();

	switch (type)
	{
		case scores_type::best:
		{
			query.from.add(dynamic_join(users_table).on(scores_table.user_id == users_table.id));
			query.where.add(without_table_check(scores_table.pp > 0 and users_table.id == id and scores_table.play_mode == user_mode and users_table.is_public == true and scores_table.is_relax == isRelax));
			query.order_by.add(scores_table.pp.desc());
			break;
		}
		case scores_type::recent:
		{
			query.from.add(dynamic_join(users_table).on(scores_table.user_id == users_table.id));
			query.where.add(without_table_check(scores_table.play_mode == user_mode and users_table.id == id and users_table.is_public == true and scores_table.is_relax == isRelax));
			query.order_by.add(scores_table.id.desc());
			break;
		}
		case scores_type::first:
		{
			const tables::scores_first scores_first_table{};
			query.from.add(dynamic_join(scores_first_table).on(scores_first_table.beatmap_md5 == scores_table.beatmap_md5));
			query.where.add(without_table_check(scores_first_table.user_id == id and scores_first_table.play_mode == user_mode and scores_first_table.is_relax == isRelax));
			query.order_by.add(scores_table.time.desc());
			break;
		}
	}

	std::pair<uint32_t, uint32_t> limit = sql_helper::paginate(page, length, 100);
	auto q = query.offset(limit.first).limit(limit.second);
	auto result = db(q);

	if (result.empty())
	{
		return this->createResponse(Status::CODE_204, "[]");
	}

	for (const auto& row : result)
	{
		json score;

		score["id"] = row.id.value();
		score["rank"] = row.ranking.value();
		score["score"] = row.score.value();
		score["max_combo"] = row.score_t.value();
		score["full_combo"] = row.full_combo.value();
		score["mods"] = row.mods.value();
		score["count_300"] = row.count_300.value();
		score["count_100"] = row.count_100.value();
		score["count_50"] = row.count_50.value();
		score["count_geki"] = row.count_gekis.value();
		score["count_katu"] = row.count_katus.value();
		score["count_miss"] = row.count_misses.value();
		score["time"] = aru::time_convert::get_date(row.time);
		score["mode"] = row.play_mode.value();
		score["accuracy"] = row.accuracy.value();
		score["pp"] = row.pp.value();

		json beatmap;
		beatmap["beatmap_id"] = row.beatmap_id.value();
		beatmap["beatmapset_id"] = row.beatmapset_id.value();
		beatmap["beatmap_md5"] = row.beatmap_md5.value();
		beatmap["artist"] = row.artist.value();
		beatmap["title"] = row.title.value();
		beatmap["difficulty_name"] = row.difficulty_name.value();
		beatmap["cs"] = row.cs.value();
		beatmap["ar"] = row.ar.value();
		beatmap["od"] = row.od.value();
		beatmap["hp"] = row.hp.value();
		switch (user_mode)
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

		beatmap["difficulty_full"] = diff;
		beatmap["max_combo"] = row.beatmap_t.value();
		beatmap["hit_length"] = row.hit_length.value();
		beatmap["ranked_status"] = row.ranked_status.value();
		beatmap["ranked_status_frozen"] = row.ranked_status_freezed.value();
		beatmap["latest_update"] = row.latest_update.value();

		score["beatmap"] = beatmap;

		response.push_back(score);
	}

	return createResponse(Status::CODE_200, response.dump().c_str());
}

int32_t users_controller::get_mode(const aru::database& db, int32_t id) const
{
	const tables::users users_table{};
	auto result = db(sqlpp::select(users_table.favourite_mode).from(users_table).where(users_table.id == id));

	if (result.empty())
	{
		return -1; // player doesn't exist
	}

	const auto& res = result.front();
	return res.favourite_mode;
}
