#ifndef MainController_hpp
#define MainController_hpp

#include "Globals.h"
#include "dto/DTOs.hpp"

#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"
#include "database/tables/UsersTable.h"

#include OATPP_CODEGEN_BEGIN(ApiController) //<-- Begin codegen

/**
 * Sample Api Controller.
 */
class MainController : public oatpp::web::server::api::ApiController {
private:
	typedef MainController __ControllerType;
public:
	/**
	 * Constructor with object mapper.
	 * @param objectMapper - default object mapper used to serialize/deserialize DTOs.
	 */
	MainController(OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper))
		: oatpp::web::server::api::ApiController(objectMapper)
	{}
public:

	ENDPOINT("GET", "/ping", Ping)
	{
		DefaultDTO::Wrapper pong = DefaultDTO::createShared();
		pong->statusCode = 200;
		pong->message = himitsu::memes[rand() % himitsu::memes.size()].c_str();
		return createDtoResponse(Status::CODE_200, pong);
	};

	ENDPOINT("GET", "/leaderboard", Leaderboard, 
		QUERY(Int32, user_mode, "mode", "0"), QUERY(Int32, relax, "relax", "0"), QUERY(Int32, page, "page", "0"), QUERY(Int32, length, "length", "50"), QUERY(String, country, "country", ""))
	{
		std::string mode = himitsu::osu::modeToString(user_mode);
		std::string table = "users_stats";
		std::string key = "ripple:leaderboard:" + mode;
		if (country != "")
		{
			std::string c = country->c_str();
			key += ":" + c;
		}
		if (relax == 1) { key += ":relax"; }

		OATPP_COMPONENT(std::shared_ptr<himitsu::redis>, m_redis);
		std::vector<cpp_redis::reply> range;

		try
		{
			m_redis->get()->zrevrange(key, page * length, page * length + length + 1, false, [&](cpp_redis::reply& reply)
			{
				if (reply)
					range = reply.as_array();
			});
			m_redis->get()->sync_commit();
		}
		catch (...)
		{
			range.~vector();
			return error500();
		};

		if (range.empty())
		{
			range.~vector();
			json response;
			response["statusCode"] = 200;
			response["users"] = json::array();

			auto w = createResponse(Status::CODE_200, response.dump().c_str());
			CORS_SUPPORT(w);
			return w;
		}

		std::string users_vec;
		std::for_each(range.begin(), range.end(), [&](cpp_redis::reply& str) { users_vec += str.as_string() + ","; });
		users_vec.pop_back(); // remove ',' after for_each

		users u_table{};
		himitsu::Connection db = himitsu::ConnectionPool::createConnection();
		json response;
		response["users"] = json::array();
		response["statusCode"] = 200;

		if (relax == 1)
		{
			users_stats_relax us_table{};
			auto query = sqlpp::dynamic_select(*db)
				.dynamic_columns(u_table.id, u_table.username, u_table.country)
				.from(u_table.join(us_table).on(u_table.id == us_table.id))
				.dynamic_order_by().unconditionally();
			switch (user_mode)
			{
				default:
				{
					query.selected_columns.add(without_table_check(us_table.ranked_score_std));
					query.selected_columns.add(without_table_check(us_table.playcount_std));
					query.selected_columns.add(without_table_check(us_table.avg_accuracy_std));
					query.selected_columns.add(without_table_check(us_table.pp_std));
					query.order_by.add(us_table.pp_std.desc());
					break;
				}
				case 1:
				{
					query.selected_columns.add(without_table_check(us_table.ranked_score_taiko));
					query.selected_columns.add(without_table_check(us_table.playcount_taiko));
					query.selected_columns.add(without_table_check(us_table.avg_accuracy_taiko));
					query.selected_columns.add(without_table_check(us_table.pp_taiko));
					query.order_by.add(us_table.pp_taiko.desc());
					break;
				}
				case 2:
				{
					query.selected_columns.add(without_table_check(us_table.ranked_score_ctb));
					query.selected_columns.add(without_table_check(us_table.playcount_ctb));
					query.selected_columns.add(without_table_check(us_table.avg_accuracy_ctb));
					query.selected_columns.add(without_table_check(us_table.pp_ctb));
					query.order_by.add(us_table.pp_ctb.desc());
					break;
				}
				case 3:
				{
					query.selected_columns.add(without_table_check(us_table.ranked_score_mania));
					query.selected_columns.add(without_table_check(us_table.playcount_mania));
					query.selected_columns.add(without_table_check(us_table.avg_accuracy_mania));
					query.selected_columns.add(without_table_check(us_table.pp_mania));
					query.order_by.add(us_table.pp_mania.desc());
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
				user["global_rank"] = m_redis->getRedisRank(fmt::format("ripple:leaderboard:{0}{1}", mode, relax == 1 ? ":relax" : ""), std::to_string(id));

				response["users"].push_back(user);
			}
		}
		else
		{
			users_stats us_table{};
			auto query = sqlpp::dynamic_select(*db)
				.dynamic_columns(u_table.id, u_table.username, u_table.country)
				.from(u_table.join(us_table).on(u_table.id == us_table.id))
				.dynamic_order_by().unconditionally();
			switch (user_mode)
			{
				default:
				{
					query.selected_columns.add(without_table_check(us_table.ranked_score_std));
					query.selected_columns.add(without_table_check(us_table.playcount_std));
					query.selected_columns.add(without_table_check(us_table.avg_accuracy_std));
					query.selected_columns.add(without_table_check(us_table.pp_std));
					query.order_by.add(us_table.pp_std.desc());
					break;
				}
				case 1:
				{
					query.selected_columns.add(without_table_check(us_table.ranked_score_taiko));
					query.selected_columns.add(without_table_check(us_table.playcount_taiko));
					query.selected_columns.add(without_table_check(us_table.avg_accuracy_taiko));
					query.selected_columns.add(without_table_check(us_table.pp_taiko));
					query.order_by.add(us_table.pp_taiko.desc());
					break;
				}
				case 2:
				{
					query.selected_columns.add(without_table_check(us_table.ranked_score_ctb));
					query.selected_columns.add(without_table_check(us_table.playcount_ctb));
					query.selected_columns.add(without_table_check(us_table.avg_accuracy_ctb));
					query.selected_columns.add(without_table_check(us_table.pp_ctb));
					query.order_by.add(us_table.pp_ctb.desc());
					break;
				}
				case 3:
				{
					query.selected_columns.add(without_table_check(us_table.ranked_score_mania));
					query.selected_columns.add(without_table_check(us_table.playcount_mania));
					query.selected_columns.add(without_table_check(us_table.avg_accuracy_mania));
					query.selected_columns.add(without_table_check(us_table.pp_mania));
					query.order_by.add(us_table.pp_mania.desc());
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
				user["global_rank"] = m_redis->getRedisRank(fmt::format("ripple:leaderboard:{0}{1}", mode, relax == 1 ? ":relax" : ""), std::to_string(id));

				response["users"].push_back(user);
			}
		}

		range.~vector();

		auto w = createResponse(Status::CODE_200, response.dump().c_str());
		CORS_SUPPORT(w);
		return w;
	};

};

#include OATPP_CODEGEN_BEGIN(ApiController) //<-- End codegen

#endif // !MainController_hpp
