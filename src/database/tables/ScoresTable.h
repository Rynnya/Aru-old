#ifndef ScoresTable
#define ScoresTable

#include "CoreTable.h"

struct scores_objects
{
	object_struct(id, sqlpp::integer);
	object_struct(beatmap_md5, sqlpp::varchar);
	object_struct(userid, sqlpp::integer);
	object_struct(score, sqlpp::bigint);
	object_struct(max_combo, sqlpp::integer);
	object_struct(full_combo, sqlpp::boolean);
	object_struct(mods, sqlpp::integer);
	object_struct(count_300, sqlpp::integer);
	object_struct(count_100, sqlpp::integer);
	object_struct(count_50, sqlpp::integer);
	object_struct(count_katus, sqlpp::integer);
	object_struct(count_gekis, sqlpp::integer);
	object_struct(count_misses, sqlpp::integer);
	object_struct(time, sqlpp::bigint);
	object_struct(play_mode, sqlpp::integer);
	object_struct(completed, sqlpp::integer);
	object_struct(accuracy, sqlpp::floating_point);
	object_struct(pp, sqlpp::floating_point);
	object_struct(playtime, sqlpp::integer);
	object_struct(is_relax, sqlpp::boolean);
};

struct scores_first_objects
{
	object_struct(scoreid, sqlpp::integer);
	object_struct(beatmap_md5, sqlpp::varchar);
	object_struct(userid, sqlpp::integer);
	object_struct(play_mode, sqlpp::integer);
	object_struct(is_relax, sqlpp::boolean);
};

database_table(scores, 
	scores_objects::id,
	scores_objects::beatmap_md5,
	scores_objects::userid,
	scores_objects::score,
	scores_objects::max_combo,
	scores_objects::full_combo,
	scores_objects::mods,
	scores_objects::count_300,
	scores_objects::count_100,
	scores_objects::count_50,
	scores_objects::count_katus,
	scores_objects::count_gekis,
	scores_objects::count_misses,
	scores_objects::time,
	scores_objects::play_mode,
	scores_objects::completed,
	scores_objects::accuracy,
	scores_objects::pp,
	scores_objects::playtime,
	scores_objects::is_relax
);

database_table(scores_first,
	scores_first_objects::scoreid,
	scores_first_objects::beatmap_md5,
	scores_first_objects::userid,
	scores_first_objects::play_mode,
	scores_first_objects::is_relax
);

#endif // !BeatmapTable
