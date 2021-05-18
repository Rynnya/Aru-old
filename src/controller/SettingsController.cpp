#include "SettingsController.hpp"

#include "database/tables/OtherTable.hpp"
#include "database/tables/UsersTable.hpp"

std::shared_ptr<SettingsController::OutgoingResponse> SettingsController::getSettings(Int32 id) const
{
	auto db = himitsu::ConnectionPool::getInstance()->getConnection();
	users user{};
	users_preferences pref{};

	auto result = (**db)(sqlpp::select(
		user.id, user.favourite_mode, user.favourite_relax, user.play_style,
		pref.scoreboard_display_classic, pref.scoreboard_display_relax,
		pref.auto_last_classic, pref.auto_last_relax,
		pref.score_overwrite_std, pref.score_overwrite_taiko, pref.score_overwrite_ctb, pref.score_overwrite_mania
	).from(user.join(pref).on(user.id == pref.id)).where(user.id == (*id)).limit(1u));

	if (result.empty())
	{
		return createResponse(Status::CODE_500,
			himitsu::createError(Status::CODE_500, "How this happend? We forgot to remove token?").c_str()
		);
	}

	const auto& row = result.front();
	json response;
	response["id"]            = row.id.value();
	response["default_mode"]  = row.favourite_mode.value();
	response["default_relax"] = row.favourite_relax.value();
	response["play_style"]    = row.play_style.value();

	response["scoreboard_display_vanilla"] = row.scoreboard_display_classic.value();
	response["scoreboard_display_relax"]   = row.scoreboard_display_relax.value();
	response["auto_last_vanilla"]          = row.auto_last_classic.value();
	response["auto_last_relax"]            = row.auto_last_relax.value();

	json score;
	score["std"]   = row.score_overwrite_std.value();
	score["taiko"] = row.score_overwrite_taiko.value();
	score["ctb"]   = row.score_overwrite_ctb.value();
	score["mania"] = row.score_overwrite_mania.value();
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
		{
			return createResponse(Status::CODE_400, 
				himitsu::createError(Status::CODE_400, "No data provided").c_str()
			);
		}

		auto db = himitsu::ConnectionPool::getInstance()->getConnection();
		users user{};

		auto upd = (*db)->prepare(sqlpp::update(user).set(user.background = sqlpp::parameter(user.background)).where(user.id == userID));
		upd.params.background = jsonRoot["background"].get<std::string>();
		(**db)(upd);

		auto response = createResponse(Status::CODE_200, "OK");
		response->putHeader("Content-Type", "text/plain");
		return response;
	}

	return createResponse(Status::CODE_400,
		himitsu::createError(Status::CODE_400, "No data provided").c_str()
	);
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
		{
			return createResponse(Status::CODE_400,
				himitsu::createError(Status::CODE_400, "No data provided").c_str()
			);
		}

		auto db = himitsu::ConnectionPool::getInstance()->getConnection();
		users user{};

		auto upd = (*db)->prepare(sqlpp::update(user).set(user.userpage = sqlpp::parameter(user.background)).where(user.id == userID));
		upd.params.background = jsonRoot["userpage"].get<std::string>();
		(**db)(upd);

		auto response = createResponse(Status::CODE_200, "OK");
		response->putHeader("Content-Type", "text/plain");
		return response;
	}

	return createResponse(Status::CODE_400,
		himitsu::createError(Status::CODE_400, "No data provided").c_str()
	);
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
		{
			return createResponse(Status::CODE_400,
				himitsu::createError(Status::CODE_400, "No data provided").c_str()
			);
		}

		auto db = himitsu::ConnectionPool::getInstance()->getConnection();
		users user{};

		auto upd = (*db)->prepare(sqlpp::update(user).set(user.status = sqlpp::parameter(user.background)).where(user.id == userID));
		upd.params.background = jsonRoot["status"].get<std::string>();
		(**db)(upd);

		auto response = createResponse(Status::CODE_200, "OK");
		response->putHeader("Content-Type", "text/plain");
		return response;
	}

	return createResponse(Status::CODE_400,
		himitsu::createError(Status::CODE_400, "No data provided").c_str()
	);
}

std::shared_ptr<SettingsController::OutgoingResponse> SettingsController::updatePlayStyle(Int32 id, int body) const
{
	if (body > 16)
		return createResponse(Status::CODE_400,
			himitsu::createError(Status::CODE_400, "PlayStyle cannot be more than 15").c_str()
		);

	if (body < 0)
		return createResponse(Status::CODE_400,
			himitsu::createError(Status::CODE_400, "PlayStyle cannot be lower than 0").c_str()
		);

	auto db = himitsu::ConnectionPool::getInstance()->getConnection();
	users u_table{};

	(**db)(sqlpp::update(u_table).set(u_table.play_style = body).where(u_table.id == (*id)));
	
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

	auto db = himitsu::ConnectionPool::getInstance()->getConnection();
	users_preferences u_pref{};

	using namespace himitsu;
	(**db)(sqlpp::update(u_pref).set(
		u_pref.scoreboard_display_classic = utils::intToBoolean(pref & 1, true),
		u_pref.scoreboard_display_relax = utils::intToBoolean(pref & 2, true),
		u_pref.auto_last_classic = auto_classic,
		u_pref.auto_last_relax = auto_relax,
		u_pref.score_overwrite_std = utils::intToBoolean(pref & 4, true),
		u_pref.score_overwrite_taiko = utils::intToBoolean(pref & 8, true),
		u_pref.score_overwrite_ctb = utils::intToBoolean(pref & 16, true),
		u_pref.score_overwrite_mania = utils::intToBoolean(pref & 32, true)
	).where(u_pref.id == (*id)));

	auto response = createResponse(Status::CODE_200, "OK");
	response->putHeader("Content-Type", "text/plain");
	return response;
}