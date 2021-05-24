#include "SettingsController.hpp"

#include "database/tables/OtherTable.hpp"
#include "database/tables/UsersTable.hpp"

std::shared_ptr<SettingsController::OutgoingResponse> SettingsController::getSettings(Int32 id) const
{
	auto db(himitsu::ConnectionPool::getInstance()->getConnection());
	const tables::users users_table{};
	const tables::users_preferences users_preferences_table{};

	auto result = (*db)(sqlpp::select(
		users_table.id, users_table.favourite_mode, users_table.favourite_relax, users_table.play_style, users_table.is_relax,
		users_preferences_table.scoreboard_display_classic, users_preferences_table.scoreboard_display_relax,
		users_preferences_table.auto_last_classic, users_preferences_table.auto_last_relax,
		users_preferences_table.score_overwrite_std, users_preferences_table.score_overwrite_taiko,
		users_preferences_table.score_overwrite_ctb, users_preferences_table.score_overwrite_mania
	).from(users_table.join(users_preferences_table).on(users_table.id == users_preferences_table.id)).where(users_table.id == (*id)).limit(1u));

	if (result.empty())
		return createResponse(Status::CODE_500, himitsu::createError(Status::CODE_500, "How this happend? We forgot to remove token?").c_str());

	const auto& row = result.front();
	json response;
	response["id"]            = row.id.value();
	response["submode"]       = (int)row.is_relax;
	response["default_mode"]  = row.favourite_mode.value();
	response["default_relax"] = (int)row.favourite_relax;
	response["playstyle"]     = row.play_style.value();

	response["scoreboard_display_vanilla"] = (int)row.scoreboard_display_classic;
	response["scoreboard_display_relax"]   = (int)row.scoreboard_display_relax;
	response["auto_last_vanilla"]          = row.auto_last_classic.value();
	response["auto_last_relax"]            = row.auto_last_relax.value();

	json score;
	score["std"]   = (int)row.score_overwrite_std;
	score["taiko"] = (int)row.score_overwrite_taiko;
	score["ctb"]   = (int)row.score_overwrite_ctb;
	score["mania"] = (int)row.score_overwrite_mania;
	response["overwrite"] = score;

	return createResponse(Status::CODE_200, response.dump().c_str());
}

std::shared_ptr<SettingsController::OutgoingResponse> SettingsController::updateBackground(
	Int32 id,
	std::string request
) const
{
	int userID = id;
	json jsonRoot = json::parse(request, nullptr, false);
	if (!jsonRoot.is_discarded())
	{
		if (jsonRoot["background"].is_null())
			return createResponse(Status::CODE_400, himitsu::createError(Status::CODE_400, "No data provided").c_str());

		auto db(himitsu::ConnectionPool::getInstance()->getConnection());
		const tables::users users_table{};

		auto upd = db->prepare(sqlpp::update(users_table)
			.set(users_table.background = sqlpp::parameter(users_table.background)).where(users_table.id == userID));
		upd.params.background = jsonRoot["background"].get<std::string>();
		(*db)(upd);

		auto response = createResponse(Status::CODE_200, "OK");
		response->putHeader("Content-Type", "text/plain");
		return response;
	}

	return createResponse(Status::CODE_400, himitsu::createError(Status::CODE_400, "No data provided").c_str());
}

std::shared_ptr<SettingsController::OutgoingResponse> SettingsController::updateUserpage(
	Int32 id,
	std::string request
) const
{
	int userID = id;
	json jsonRoot = json::parse(request, nullptr, false);
	if (!jsonRoot.is_discarded())
	{
		if (jsonRoot["userpage"].is_null())
			return createResponse(Status::CODE_400, himitsu::createError(Status::CODE_400, "No data provided").c_str());

		auto db(himitsu::ConnectionPool::getInstance()->getConnection());
		const tables::users users_table{};

		auto upd = db->prepare(sqlpp::update(users_table)
			.set(users_table.userpage = sqlpp::parameter(users_table.background)).where(users_table.id == userID));
		upd.params.background = jsonRoot["userpage"].get<std::string>();
		(*db)(upd);

		auto response = createResponse(Status::CODE_200, "OK");
		response->putHeader("Content-Type", "text/plain");
		return response;
	}

	return createResponse(Status::CODE_400, himitsu::createError(Status::CODE_400, "No data provided").c_str());
}

std::shared_ptr<SettingsController::OutgoingResponse> SettingsController::updateStatus(
	Int32 id,
	std::string request
) const
{
	int userID = id;
	json jsonRoot = json::parse(request, nullptr, false);
	if (!jsonRoot.is_discarded())
	{
		if (jsonRoot["status"].is_null())
			return createResponse(Status::CODE_400, himitsu::createError(Status::CODE_400, "No data provided").c_str());

		auto db(himitsu::ConnectionPool::getInstance()->getConnection());
		const tables::users users_table{};

		auto upd = db->prepare(sqlpp::update(users_table)
			.set(users_table.status = sqlpp::parameter(users_table.background)).where(users_table.id == userID));
		upd.params.background = jsonRoot["status"].get<std::string>();
		(*db)(upd);

		auto response = createResponse(Status::CODE_200, "OK");
		response->putHeader("Content-Type", "text/plain");
		return response;
	}

	return createResponse(Status::CODE_400, himitsu::createError(Status::CODE_400, "No data provided").c_str());
}

std::shared_ptr<SettingsController::OutgoingResponse> SettingsController::updatePref(Int32 id, int fav_mode, bool fav_relax, int playstyle) const
{
	if (fav_mode == 3 && fav_relax)
		return createResponse(Status::CODE_404, himitsu::createError(Status::CODE_404, "Mania don't have relax mode").c_str());
	if (fav_mode > 3 || fav_mode < 0) fav_mode = 0;
	if (playstyle > 15 || playstyle < 0) playstyle = 0;

	auto db(himitsu::ConnectionPool::getInstance()->getConnection());
	const tables::users users_table{};

	(*db)(sqlpp::update(users_table).set(
		users_table.favourite_mode = fav_mode,
		users_table.favourite_relax = fav_relax,
		users_table.play_style = playstyle
	).where(users_table.id == (*id)));

	auto response = createResponse(Status::CODE_200, "OK");
	response->putHeader("Content-Type", "text/plain");
	return response;
}

std::shared_ptr<SettingsController::OutgoingResponse> SettingsController::updateScoreboard(
	Int32 id,
	int pref,
	int auto_classic,
	int auto_relax
) const
{
	if (auto_classic > 2 || auto_classic < 0) auto_classic = 0;
	if (auto_relax > 2 || auto_relax < 0) auto_relax = 0;

	auto db(himitsu::ConnectionPool::getInstance()->getConnection());
	const tables::users_preferences users_preferences_table{};
	const tables::users users_table{};

	using namespace himitsu;
	(*db)(sqlpp::update(users_preferences_table).set(
		users_preferences_table.scoreboard_display_classic = utils::intToBoolean(pref & 1, true),
		users_preferences_table.scoreboard_display_relax   = utils::intToBoolean(pref & 2, true),
		users_preferences_table.auto_last_classic          = auto_classic,
		users_preferences_table.auto_last_relax            = auto_relax,
		users_preferences_table.score_overwrite_std        = utils::intToBoolean(pref & 4, true),
		users_preferences_table.score_overwrite_taiko      = utils::intToBoolean(pref & 8, true),
		users_preferences_table.score_overwrite_ctb        = utils::intToBoolean(pref & 16, true),
		users_preferences_table.score_overwrite_mania      = utils::intToBoolean(pref & 32, true)
	).where(users_preferences_table.id == (*id)));

	(*db)(sqlpp::update(users_table).set(users_table.is_relax = utils::intToBoolean(pref & 64, true)).where(users_table.id == (*id)));

	auto response = createResponse(Status::CODE_200, "OK");
	response->putHeader("Content-Type", "text/plain");
	return response;
}