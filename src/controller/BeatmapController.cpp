#include "BeatmapController.hpp"

void BeatmapController::logRankChange(aru::Connection db, int32_t id, int64_t bid, int32_t status, std::string type)
{
	const tables::ranking_logs logs_table {};
	db(sqlpp::insert_into(logs_table).set(
		logs_table.map_id = bid,
		logs_table.modified_by = id,
		logs_table.status = status,
		logs_table.map_type = type
	));
}
