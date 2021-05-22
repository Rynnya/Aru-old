#include "UsersController.hpp"

#include "database/tables/BeatmapTable.hpp"
#include "database/tables/ScoresTable.hpp"

std::shared_ptr<UsersController::OutgoingResponse> UsersController::buildScores(Int32 id, Int32 user_mode, Int32 relax, Int32 page, Int32 length, scores_type type) const
{
	std::string mode;
	if (user_mode == -1 && !getMode(id, &mode))
		return createResponse(Status::CODE_404, himitsu::createError(Status::CODE_404, "Player not found").c_str());

	length = SQLHelper::Limitize(1, length, 100);

	if (relax == 1 && mode == "mania")
		return createResponse(Status::CODE_404, himitsu::createError(Status::CODE_404, "Mania don't have relax mode").c_str());

	auto db(himitsu::ConnectionPool::getInstance()->getConnection());

	json response = json::array();

	beatmaps b_table{};
	bool isRelax = himitsu::utils::intToBoolean(relax);

	scores s_table{};
	users u_table{};
	auto query = sqlpp::dynamic_select(*db, s_table.id, s_table.score, s_table.full_combo, s_table.mods,
		s_table.count_300, s_table.count_100, s_table.count_50, s_table.count_gekis, s_table.count_katus, s_table.count_misses,
		s_table.time, s_table.play_mode, s_table.accuracy, s_table.pp, s_table.completed,
		s_table.max_combo.as(score_t),

		b_table.beatmap_id, b_table.beatmapset_id, b_table.artist, b_table.title, b_table.difficulty_name,
		b_table.cs, b_table.ar, b_table.od, b_table.hp, b_table.difficulty_std, b_table.difficulty_taiko,
		b_table.difficulty_ctb, b_table.difficulty_mania, b_table.hit_length, b_table.ranked,
		b_table.ranked_status_freezed, b_table.latest_update,
		b_table.beatmap_md5, b_table.max_combo.as(beatmap_t)
	).dynamic_from(s_table.join(b_table).on(s_table.beatmap_md5 == b_table.beatmap_md5)).dynamic_where(s_table.is_relax == isRelax).dynamic_order_by();

	switch (type)
	{
		case scores_type::Best:
		{
			query.from.add(dynamic_join(u_table).on(s_table.userid == u_table.id));
			query.where.add(without_table_check(s_table.completed == 3 and s_table.pp > 0 and u_table.id == (*id) and s_table.play_mode == (*user_mode) and u_table.is_public == true));
			query.order_by.add(s_table.pp.desc());
			break;
		}
		case scores_type::Recent:
		{
			query.from.add(dynamic_join(u_table).on(s_table.userid == u_table.id));
			query.where.add(without_table_check(s_table.play_mode == (*user_mode) and u_table.id == (*id) and u_table.is_public == true));
			query.order_by.add(s_table.id.desc());
			break;
		}
		case scores_type::First:
		{
			scores_first sf_table{};
			query.from.add(dynamic_join(sf_table).on(sf_table.beatmap_md5 == s_table.beatmap_md5));
			query.where.add(without_table_check(sf_table.userid == (*id) and sf_table.play_mode == (*user_mode) and sf_table.is_relax == isRelax));
			query.order_by.add(s_table.time.desc());
			break;
		}
	}

	std::pair<unsigned int, unsigned int> limit = SQLHelper::Paginate(page, length, 100);
	auto q = query.offset(limit.first).limit(limit.second);
	auto result = (*db)(q);

	if (result.empty())
	{
		return this->createResponse(Status::CODE_200, json::array().dump().c_str());
	}

	for (const auto& row : result)
	{
		json score;

		score["id"] = row.id.value();
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
		score["time"] = himitsu::time_convert::getDate(row.time);
		score["mode"] = row.play_mode.value();
		score["accuracy"] = row.accuracy.value();
		score["pp"] = row.pp.value();
		score["completed"] = row.completed.value();

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
		beatmap["ranked"] = row.ranked.value();
		beatmap["ranked_status_frozen"] = row.ranked_status_freezed.value();
		beatmap["latest_update"] = row.latest_update.value();

		score["beatmap"] = beatmap;

		response.push_back(score);
	}

	return createResponse(Status::CODE_200, response.dump().c_str());
}

bool UsersController::getMode(Int32 id, std::string* ans) const
{
	auto db(himitsu::ConnectionPool::getInstance()->getConnection());
	users user{};
	auto result = (*db)(sqlpp::select(user.favourite_mode).from(user).where(user.id == (*id)));

	if (result.empty())
		return false; // player doesn't exist

	const auto& res = result.front();
	*ans = himitsu::osu::modeToString(res.favourite_mode);
	return true;
}