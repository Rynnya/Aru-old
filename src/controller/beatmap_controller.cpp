#include "beatmap_controller.hpp"

void beatmap_controller::log_rank_change(const aru::database& db, int32_t id, int64_t bid, int32_t status, std::string type) const
{
	const tables::ranking_logs logs_table {};
	db(sqlpp::insert_into(logs_table).set(
		logs_table.map_id = bid,
		logs_table.modified_by = id,
		logs_table.status = status,
		logs_table.map_type = type
	));
}
