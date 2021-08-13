#include "settings_controller.hpp"

#include "database/tables/other_table.hpp"
#include "database/tables/users_table.hpp"

std::shared_ptr<settings_controller::OutgoingResponse> settings_controller::get_settings(const aru::database& db, int32_t id) const
{
	const tables::users users_table{};
	const tables::users_preferences users_preferences_table{};

	auto result = db(sqlpp::select(
		users_table.id, users_table.favourite_mode, users_table.favourite_relax, users_table.play_style, users_table.is_relax,
		users_preferences_table.scoreboard_display_classic, users_preferences_table.scoreboard_display_relax,
		users_preferences_table.auto_last_classic, users_preferences_table.auto_last_relax,
		users_preferences_table.score_overwrite_std, users_preferences_table.score_overwrite_taiko,
		users_preferences_table.score_overwrite_ctb, users_preferences_table.score_overwrite_mania
	).from(users_table.join(users_preferences_table).on(users_table.id == users_preferences_table.id)).where(users_table.id == id).limit(1u));

	if (result.empty())
	{
		auto error = aru::create_error(Status::CODE_500, "How this happend? We forgot to remove token?");
		return createResponse(Status::CODE_500, error);
	}

	const auto& row = result.front();
	json response;

	response["id"]            = row.id.value();
	response["submode"]       = (int32_t)row.is_relax;
	response["default_mode"]  = row.favourite_mode.value();
	response["default_relax"] = (int32_t)row.favourite_relax;
	response["play_style"]     = row.play_style.value();

	response["scoreboard_display_vanilla"] = (int32_t)row.scoreboard_display_classic;
	response["scoreboard_display_relax"]   = (int32_t)row.scoreboard_display_relax;
	response["auto_last_vanilla"]          = row.auto_last_classic.value();
	response["auto_last_relax"]            = row.auto_last_relax.value();

	json score;
	score["std"]   = (int32_t)row.score_overwrite_std;
	score["taiko"] = (int32_t)row.score_overwrite_taiko;
	score["ctb"]   = (int32_t)row.score_overwrite_ctb;
	score["mania"] = (int32_t)row.score_overwrite_mania;
	response["overwrite"] = score;

	return createResponse(Status::CODE_200, response.dump().c_str());
}

std::shared_ptr<settings_controller::OutgoingResponse> settings_controller::update_background(
	const aru::database& db,
	int32_t id,
	std::string request
) const
{
	int32_t userID = id;
	json jsonRoot = json::parse(request, nullptr, false);
	if (!jsonRoot.is_discarded())
	{
		if (jsonRoot["background"].is_null())
		{
			auto error = aru::create_error(Status::CODE_400, "No data provided");
			return createResponse(Status::CODE_400, error);
		}

		const tables::users users_table{};
		db(sqlpp::update(users_table).set(users_table.background = u8"" + jsonRoot["background"].get<std::string>()).where(users_table.id == userID));

		auto response = createResponse(Status::CODE_200, "OK");
		response->putHeader("Content-Type", "text/plain");
		return response;
	}

	return createResponse(Status::CODE_400, aru::create_error(Status::CODE_400, "No data provided"));
}

std::shared_ptr<settings_controller::OutgoingResponse> settings_controller::update_userpage(
	const aru::database& db,
	int32_t id,
	std::string request
) const
{
	int32_t userID = id;
	json jsonRoot = json::parse(request, nullptr, false);
	if (!jsonRoot.is_discarded())
	{
		if (jsonRoot["userpage"].is_null())
		{
			auto error = aru::create_error(Status::CODE_400, "No data provided");
			return createResponse(Status::CODE_400, error);
		}

		const tables::users users_table{};
		db(sqlpp::update(users_table).set(users_table.userpage = u8"" + jsonRoot["userpage"].get<std::string>()).where(users_table.id == userID));

		auto response = createResponse(Status::CODE_200, "OK");
		response->putHeader("Content-Type", "text/plain");
		return response;
	}

	return createResponse(Status::CODE_400, aru::create_error(Status::CODE_400, "No data provided"));
}

std::shared_ptr<settings_controller::OutgoingResponse> settings_controller::update_status(
	const aru::database& db,
	int32_t id,
	std::string request
) const
{
	int32_t userID = id;
	json jsonRoot = json::parse(request, nullptr, false);
	if (!jsonRoot.is_discarded())
	{
		if (jsonRoot["status"].is_null())
		{
			auto error = aru::create_error(Status::CODE_400, "No data provided");
			return createResponse(Status::CODE_400, error);
		}

		const tables::users users_table{};
		db(sqlpp::update(users_table).set(users_table.status = u8"" + jsonRoot["status"].get<std::string>()).where(users_table.id == userID));

		auto response = createResponse(Status::CODE_200, "OK");
		response->putHeader("Content-Type", "text/plain");
		return response;
	}

	return createResponse(Status::CODE_400, aru::create_error(Status::CODE_400, "No data provided"));
}

std::shared_ptr<settings_controller::OutgoingResponse> settings_controller::update_preferences(
	const aru::database& db,
	int32_t id,
	json body
) const
{
	int32_t favourite_mode = -1;
	bool favourite_relax = false;

    const tables::users users_table{};

	auto data = db(sqlpp::select(users_table.favourite_mode, users_table.favourite_relax).from(users_table).where(users_table.id == id).limit(1u));
	if (data.empty())
	{
		auto error = aru::create_error(Status::CODE_404, "Player not found");
		return createResponse(Status::CODE_404, error);
	}

	auto& check_data = data.front();

	auto query = sqlpp::dynamic_update(*db, users_table).dynamic_set().where(users_table.id == id);
	if (body["favourite_mode"].is_number_integer())
	{
		favourite_mode = body["favourite_mode"];
		aru::utils::sanitize(favourite_mode, 0, 3);
		if (check_data.favourite_relax && favourite_mode == 3)
		{
			auto error = aru::create_error(Status::CODE_404, "Mania don't have relax mode");
			return createResponse(Status::CODE_404, error);
		}

		query.assignments.add(users_table.favourite_mode = favourite_mode);
	}

	if (body["favourite_relax"].is_number_integer())
	{
		favourite_relax = aru::utils::int_to_bool(body["favourite_relax"]);
		if (favourite_relax && check_data.favourite_mode.value() == 3)
		{
			auto error = aru::create_error(Status::CODE_404, "Mania don't have relax mode");
			return createResponse(Status::CODE_404, error);
		}

		query.assignments.add(users_table.favourite_relax = favourite_relax);
	}

	if (favourite_relax == 1 && favourite_mode == 3)
	{
		auto error = aru::create_error(Status::CODE_404, "Mania don't have relax mode");
		return createResponse(Status::CODE_404, error);
	}

	if (body["play_style"].is_number_integer())
	{
		int32_t play_style = body["play_style"];
		aru::utils::sanitize(play_style, 0, 15);
		query.assignments.add(users_table.play_style = play_style);
	}

	db(query);

	auto response = createResponse(Status::CODE_200, "OK");
	response->putHeader("Content-Type", "text/plain");
	return response;
}

std::shared_ptr<settings_controller::OutgoingResponse> settings_controller::update_scoreboard(
	const aru::database& db,
	int32_t id,
	json body
) const
{
	int32_t preferences = -1;

	const tables::users_preferences users_preferences_table{};
	const tables::users users_table{};

	using namespace aru;
	auto query = sqlpp::dynamic_update(*db, users_preferences_table).dynamic_set().where(users_preferences_table.id == id);
	if (body["preferences"].is_number_integer())
	{
		preferences = body["preferences"];
		query.assignments.add(users_preferences_table.scoreboard_display_classic = utils::int_to_bool(preferences & 1, true));
		query.assignments.add(users_preferences_table.scoreboard_display_relax   = utils::int_to_bool(preferences & 2, true));
		query.assignments.add(users_preferences_table.score_overwrite_std        = utils::int_to_bool(preferences & 4, true));
		query.assignments.add(users_preferences_table.score_overwrite_taiko      = utils::int_to_bool(preferences & 8, true));
		query.assignments.add(users_preferences_table.score_overwrite_ctb        = utils::int_to_bool(preferences & 16, true));
		query.assignments.add(users_preferences_table.score_overwrite_mania      = utils::int_to_bool(preferences & 32, true));
		db(sqlpp::update(users_table).set(users_table.is_relax = utils::int_to_bool(preferences & 64, true)).where(users_table.id == id));
	}

	if (body["auto_classic"].is_number_integer())
	{
		int32_t auto_classic = body["auto_classic"];
		utils::sanitize(auto_classic, 0, 2);
		query.assignments.add(users_preferences_table.auto_last_classic = auto_classic);
	}

	if (body["auto_relax"].is_number_integer())
	{
		int32_t auto_relax = body["auto_relax"];
		utils::sanitize(auto_relax, 0, 2);
		query.assignments.add(users_preferences_table.auto_last_relax = auto_relax);
	}

	db(query);

	OATPP_COMPONENT(std::shared_ptr<cpp_redis::client>, redis);
	redis->publish("shiro.user_preferences", std::to_string(id));
	redis->commit();

	auto response = createResponse(Status::CODE_200, "OK");
	response->putHeader("Content-Type", "text/plain");
	return response;
}
