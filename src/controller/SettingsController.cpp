#include "SettingsController.hpp"

#include "database/tables/OtherTable.hpp"
#include "database/tables/UsersTable.hpp"

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
			return createResponse(Status::CODE_204, "No data provided");
		}

		auto db = himitsu::ConnectionPool::getInstance()->getConnection();
		users user{};

		auto upd = (*db)->prepare(sqlpp::update(user).set(user.background = sqlpp::parameter(user.background)).where(user.id == userID));
		upd.params.background = jsonRoot["background"].get<std::string>();
		(**db)(upd);

		return createResponse(Status::CODE_200, "OK");
	}

	return createResponse(Status::CODE_204, "No data provided");
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
			return createResponse(Status::CODE_204, "No data provided");
		}

		auto db = himitsu::ConnectionPool::getInstance()->getConnection();
		users user{};

		auto upd = (*db)->prepare(sqlpp::update(user).set(user.userpage = sqlpp::parameter(user.background)).where(user.id == userID));
		upd.params.background = jsonRoot["userpage"].get<std::string>();
		(**db)(upd);

		return createResponse(Status::CODE_200, "OK");
	}

	return createResponse(Status::CODE_204, "No data provided");
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
			return createResponse(Status::CODE_204, "No data provided");
		}

		auto db = himitsu::ConnectionPool::getInstance()->getConnection();
		users user{};

		auto upd = (*db)->prepare(sqlpp::update(user).set(user.status = sqlpp::parameter(user.background)).where(user.id == userID));
		upd.params.background = jsonRoot["status"].get<std::string>();
		(**db)(upd);

		return createResponse(Status::CODE_200, "OK");
	}

	return createResponse(Status::CODE_204, "No data provided");
}

std::shared_ptr<SettingsController::OutgoingResponse> SettingsController::updatePlayStyle(Int32 id, int body) const
{
	if (body > 16)
		return createResponse(Status::CODE_400, "PlayStyle cannot be more than 15");

	if (body < 0)
		return createResponse(Status::CODE_400, "PlayStyle cannot be lower than 0");

	auto db = himitsu::ConnectionPool::getInstance()->getConnection();
	users u_table{};

	(**db)(sqlpp::update(u_table).set(u_table.play_style = body).where(u_table.id == (*id)));
	return createResponse(Status::CODE_200, "OK");
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

	return createResponse(Status::CODE_200, "OK");
}

// Returns true if valid token, otherwise false
bool SettingsController::checkToken(int id, String token)
{
	if (!token)
		return false;
	auto db = himitsu::ConnectionPool::getInstance()->getConnection();
	tokens token_table{};
	auto query = (*db)->prepare(sqlpp::select(token_table.user)
		.from(token_table)
		.where(token_table.user == id and token_table.token == sqlpp::parameter(token_table.token)));
	query.params.token = token.get()->c_str();
	auto result = (**db)(query);
	if (result.empty())
		return false;
	return true;
}