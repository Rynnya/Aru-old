#ifndef controller_MainController_hpp_included
#define controller_MainController_hpp_included

#include "Globals.hpp"

#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"
#include "database/tables/UsersTable.hpp"

#include "cpp_redis/misc/error.hpp"

#include OATPP_CODEGEN_BEGIN(ApiController)

class MainController : public oatpp::web::server::api::ApiController {
private:
	typedef MainController __ControllerType;
public:
	MainController(OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper))
		: oatpp::web::server::api::ApiController(objectMapper)
	{}
public:

	ENDPOINT("GET", "/ping", Ping, REQUEST(std::shared_ptr<IncomingRequest>, request))
	{
		json response;
		response["message"] = himitsu::memes[rand() % himitsu::memes.size()].c_str();
		return createResponse(Status::CODE_200, response.dump().c_str());
	};

	ENDPOINT("GET", "/leaderboard", Leaderboard, 
		QUERY(Int32, user_mode, "mode", "0"), QUERY(Int32, relax, "relax", "0"), QUERY(Int32, page, "page", "0"), QUERY(Int32, length, "length", "50"), QUERY(String, country, "country", ""))
	{
		std::string mode = himitsu::osu::modeToString(user_mode);
		std::string key = "ripple:leaderboard:" + mode;
		if (country != "")
		{
			std::string c = country->c_str();
			key += ":" + c;
		}
		if (relax == 1) { key += ":relax"; }

		OATPP_COMPONENT(std::shared_ptr<himitsu::redis>, m_redis);
		std::vector<cpp_redis::reply> range;

		m_redis->get()->zrevrange(key, page * length, page * length + length + 1, false, [&](cpp_redis::reply& reply)
		{
			if (reply)
				range = reply.as_array();
		});
		m_redis->get()->sync_commit();

		if (range.empty())
		{
			range.~vector();
			return createResponse(Status::CODE_200, json::array().dump().c_str());
		}

		std::string users_vec;
		std::for_each(range.begin(), range.end(), [&](cpp_redis::reply& str) { users_vec += str.as_string() + ","; });
		users_vec.pop_back(); // remove ',' after for_each

		const tables::users users_table{};
		auto db(himitsu::ConnectionPool::getInstance()->getConnection());
		json response = json::array();

		if (relax == 1)
		{
			const tables::users_stats_relax users_stats_table{};
			auto query = sqlpp::dynamic_select(*db)
				.dynamic_columns(users_stats_table.id, users_table.username, users_table.country)
				.from(users_table.join(users_stats_table).on(users_table.id == users_stats_table.id))
				.dynamic_order_by().unconditionally();
			switch (user_mode)
			{
				default:
				{
					query.selected_columns.add(without_table_check(users_stats_table.ranked_score_std));
					query.selected_columns.add(without_table_check(users_stats_table.playcount_std));
					query.selected_columns.add(without_table_check(users_stats_table.avg_accuracy_std));
					query.selected_columns.add(without_table_check(users_stats_table.pp_std));
					query.order_by.add(users_stats_table.pp_std.desc());
					break;
				}
				case 1:
				{
					query.selected_columns.add(without_table_check(users_stats_table.ranked_score_taiko));
					query.selected_columns.add(without_table_check(users_stats_table.playcount_taiko));
					query.selected_columns.add(without_table_check(users_stats_table.avg_accuracy_taiko));
					query.selected_columns.add(without_table_check(users_stats_table.pp_taiko));
					query.order_by.add(users_stats_table.pp_taiko.desc());
					break;
				}
				case 2:
				{
					query.selected_columns.add(without_table_check(users_stats_table.ranked_score_ctb));
					query.selected_columns.add(without_table_check(users_stats_table.playcount_ctb));
					query.selected_columns.add(without_table_check(users_stats_table.avg_accuracy_ctb));
					query.selected_columns.add(without_table_check(users_stats_table.pp_ctb));
					query.order_by.add(users_stats_table.pp_ctb.desc());
					break;
				}
			}

			auto result = (*db)(query);

			for (const auto& row : result)
			{
				json user;

				int id = row.id;
				user["id"] = id;
				user["username"] = row.username.value();
				user["country"] = row.country.value();
				user["ranked_score"] = row.at(fmt::format("ranked_score_{0}", mode)).value();
				user["playcount"] = row.at(fmt::format("playcount_{0}", mode)).value();
				user["accuracy"] = row.at(fmt::format("avg_accuracy_{0}", mode)).value();
				user["pp"] = row.at(fmt::format("pp_{0}", mode)).value();
				user["global_rank"] = m_redis->getRedisRank(fmt::format("ripple:leaderboard:{0}{1}", mode, relax == 1 ? ":relax" : ""), fmt::to_string(id));

				response.push_back(user);
			}
		}
		else
		{
			const tables::users_stats users_stats_table{};
			auto query = sqlpp::dynamic_select(*db)
				.dynamic_columns(users_table.id, users_table.username, users_table.country)
				.from(users_table.join(users_stats_table).on(users_table.id == users_stats_table.id))
				.dynamic_order_by().unconditionally();
			switch (user_mode)
			{
				default:
				{
					query.selected_columns.add(without_table_check(users_stats_table.ranked_score_std));
					query.selected_columns.add(without_table_check(users_stats_table.playcount_std));
					query.selected_columns.add(without_table_check(users_stats_table.avg_accuracy_std));
					query.selected_columns.add(without_table_check(users_stats_table.pp_std));
					query.order_by.add(users_stats_table.pp_std.desc());
					break;
				}
				case 1:
				{
					query.selected_columns.add(without_table_check(users_stats_table.ranked_score_taiko));
					query.selected_columns.add(without_table_check(users_stats_table.playcount_taiko));
					query.selected_columns.add(without_table_check(users_stats_table.avg_accuracy_taiko));
					query.selected_columns.add(without_table_check(users_stats_table.pp_taiko));
					query.order_by.add(users_stats_table.pp_taiko.desc());
					break;
				}
				case 2:
				{
					query.selected_columns.add(without_table_check(users_stats_table.ranked_score_ctb));
					query.selected_columns.add(without_table_check(users_stats_table.playcount_ctb));
					query.selected_columns.add(without_table_check(users_stats_table.avg_accuracy_ctb));
					query.selected_columns.add(without_table_check(users_stats_table.pp_ctb));
					query.order_by.add(users_stats_table.pp_ctb.desc());
					break;
				}
				case 3:
				{
					query.selected_columns.add(without_table_check(users_stats_table.ranked_score_mania));
					query.selected_columns.add(without_table_check(users_stats_table.playcount_mania));
					query.selected_columns.add(without_table_check(users_stats_table.avg_accuracy_mania));
					query.selected_columns.add(without_table_check(users_stats_table.pp_mania));
					query.order_by.add(users_stats_table.pp_mania.desc());
					break;
				}
			}

			auto result = (*db)(query);

			for (const auto& row : result)
			{
				json user;

				int id = row.id;
				user["id"] = id;
				user["username"] = row.username.value();
				user["country"] = row.country.value();
				user["ranked_score"] = row.at(fmt::format("ranked_score_{0}", mode)).value();
				user["playcount"] = row.at(fmt::format("playcount_{0}", mode)).value();
				user["accuracy"] = row.at(fmt::format("avg_accuracy_{0}", mode)).value();
				user["pp"] = row.at(fmt::format("pp_{0}", mode)).value();
				user["global_rank"] = m_redis->getRedisRank(fmt::format("ripple:leaderboard:{0}{1}", mode, relax == 1 ? ":relax" : ""), fmt::to_string(id));

				response.push_back(user);
			}
		}

		range.~vector();
		return createResponse(Status::CODE_200, response.dump().c_str());
	};

};

#include OATPP_CODEGEN_END(ApiController)

#endif
