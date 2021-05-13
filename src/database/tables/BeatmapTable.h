#ifndef BeatmapTable
#define BeatmapTable

#include "CoreTable.h"

struct beatmap_objects
{
	object_struct(id, sqlpp::integer);
	object_struct(beatmap_id, sqlpp::integer);
	object_struct(beatmapset_id, sqlpp::integer);
	object_struct(beatmap_md5, sqlpp::varchar);
	object_struct(artist, sqlpp::varchar);
	object_struct(title, sqlpp::varchar);
	object_struct(difficulty_name, sqlpp::varchar);
	object_struct(creator, sqlpp::varchar);
	object_struct(file_name, sqlpp::varchar);
	object_struct(cs, sqlpp::floating_point);
	object_struct(ar, sqlpp::floating_point);
	object_struct(od, sqlpp::floating_point);
	object_struct(hp, sqlpp::floating_point);
	object_struct(mode, sqlpp::integer);
	object_struct(difficulty_std, sqlpp::floating_point);
	object_struct(difficulty_taiko, sqlpp::floating_point);
	object_struct(difficulty_ctb, sqlpp::floating_point);
	object_struct(difficulty_mania, sqlpp::floating_point);
	object_struct(max_combo, sqlpp::integer);
	object_struct(hit_length, sqlpp::integer);
	object_struct(bpm, sqlpp::integer);
	object_struct(count_normal, sqlpp::integer);
	object_struct(count_slider, sqlpp::integer);
	object_struct(count_spinner, sqlpp::integer);
	object_struct(playcount, sqlpp::integer);
	object_struct(passcount, sqlpp::integer);
	object_struct(ranked, sqlpp::integer);
	object_struct(latest_update, sqlpp::bigint);
	object_struct(ranked_status_freezed, sqlpp::boolean);
	object_struct(creating_date, sqlpp::bigint);
	object_struct(pp_100, sqlpp::integer);
	object_struct(pp_99, sqlpp::integer);
	object_struct(pp_98, sqlpp::integer);
	object_struct(pp_95, sqlpp::integer);
};

database_table(beatmaps,
	beatmap_objects::id,
	beatmap_objects::beatmap_id,
	beatmap_objects::beatmapset_id,
	beatmap_objects::beatmap_md5,
	beatmap_objects::artist,
	beatmap_objects::title,
	beatmap_objects::difficulty_name,
	beatmap_objects::creator,
	beatmap_objects::file_name,
	beatmap_objects::cs,
	beatmap_objects::ar,
	beatmap_objects::od,
	beatmap_objects::hp,
	beatmap_objects::mode,
	beatmap_objects::difficulty_std,
	beatmap_objects::difficulty_taiko,
	beatmap_objects::difficulty_ctb,
	beatmap_objects::difficulty_mania,
	beatmap_objects::max_combo,
	beatmap_objects::hit_length,
	beatmap_objects::bpm,
	beatmap_objects::count_normal,
	beatmap_objects::count_slider,
	beatmap_objects::count_spinner,
	beatmap_objects::playcount,
	beatmap_objects::passcount,
	beatmap_objects::ranked,
	beatmap_objects::latest_update,
	beatmap_objects::ranked_status_freezed,
	beatmap_objects::creating_date,
	beatmap_objects::pp_100,
	beatmap_objects::pp_99,
	beatmap_objects::pp_98,
	beatmap_objects::pp_95
);

#endif // !BeatmapTable
