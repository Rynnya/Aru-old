#ifndef database_tables_UsersTable_hpp_included
#define database_tables_UsersTable_hpp_included

#include "CoreTable.hpp"

namespace tables
{

	struct users_objects
	{
		object_struct(id, sqlpp::integer);
		object_struct(username, sqlpp::varchar);
		object_struct(safe_username, sqlpp::varchar);
		object_struct(registration_date, sqlpp::bigint);
		object_struct(latest_activity, sqlpp::bigint);
		object_struct(country, sqlpp::varchar);
		object_struct(password_md5, sqlpp::varchar);
		object_struct(salt, sqlpp::varchar);
		object_struct(email, sqlpp::varchar);
		object_struct(ip, sqlpp::varchar);
		object_struct(roles, sqlpp::integer_unsigned);
		object_struct(userpage, sqlpp::varchar);
		object_struct(background, sqlpp::varchar);
		object_struct(status, sqlpp::varchar);
		object_struct(is_public, sqlpp::boolean);
		object_struct(is_relax, sqlpp::boolean);
		object_struct(favourite_mode, sqlpp::integer);
		object_struct(favourite_relax, sqlpp::boolean);
		object_struct(play_style, sqlpp::integer);
	};

	struct users_stats_objects
	{
		object_struct(id, sqlpp::integer);
		object_struct(rank_std, sqlpp::integer);
		object_struct(rank_taiko, sqlpp::integer);
		object_struct(rank_ctb, sqlpp::integer);
		object_struct(rank_mania, sqlpp::integer);
		object_struct(ranked_score_std, sqlpp::bigint);
		object_struct(ranked_score_taiko, sqlpp::bigint);
		object_struct(ranked_score_ctb, sqlpp::bigint);
		object_struct(ranked_score_mania, sqlpp::bigint);
		object_struct(total_score_std, sqlpp::bigint);
		object_struct(total_score_taiko, sqlpp::bigint);
		object_struct(total_score_ctb, sqlpp::bigint);
		object_struct(total_score_mania, sqlpp::bigint);
		object_struct(playcount_std, sqlpp::integer);
		object_struct(playcount_taiko, sqlpp::integer);
		object_struct(playcount_ctb, sqlpp::integer);
		object_struct(playcount_mania, sqlpp::integer);
		object_struct(replays_watched_std, sqlpp::integer);
		object_struct(replays_watched_taiko, sqlpp::integer);
		object_struct(replays_watched_ctb, sqlpp::integer);
		object_struct(replays_watched_mania, sqlpp::integer);
		object_struct(total_hits_std, sqlpp::integer);
		object_struct(total_hits_taiko, sqlpp::integer);
		object_struct(total_hits_ctb, sqlpp::integer);
		object_struct(total_hits_mania, sqlpp::integer);
		object_struct(max_combo_std, sqlpp::integer);
		object_struct(max_combo_taiko, sqlpp::integer);
		object_struct(max_combo_ctb, sqlpp::integer);
		object_struct(max_combo_mania, sqlpp::integer);
		object_struct(level_std, sqlpp::integer);
		object_struct(level_taiko, sqlpp::integer);
		object_struct(level_ctb, sqlpp::integer);
		object_struct(level_mania, sqlpp::integer);
		object_struct(playtime_std, sqlpp::integer);
		object_struct(playtime_taiko, sqlpp::integer);
		object_struct(playtime_ctb, sqlpp::integer);
		object_struct(playtime_mania, sqlpp::integer);
		object_struct(avg_accuracy_std, sqlpp::integer);
		object_struct(avg_accuracy_taiko, sqlpp::integer);
		object_struct(avg_accuracy_ctb, sqlpp::integer);
		object_struct(avg_accuracy_mania, sqlpp::integer);
		object_struct(pp_std, sqlpp::integer);
		object_struct(pp_taiko, sqlpp::integer);
		object_struct(pp_ctb, sqlpp::integer);
		object_struct(pp_mania, sqlpp::integer);
		object_struct(count_A_std, sqlpp::integer);
		object_struct(count_S_std, sqlpp::integer);
		object_struct(count_X_std, sqlpp::integer);
		object_struct(count_SH_std, sqlpp::integer);
		object_struct(count_XH_std, sqlpp::integer);
		object_struct(count_A_taiko, sqlpp::integer);
		object_struct(count_S_taiko, sqlpp::integer);
		object_struct(count_X_taiko, sqlpp::integer);
		object_struct(count_SH_taiko, sqlpp::integer);
		object_struct(count_XH_taiko, sqlpp::integer);
		object_struct(count_A_ctb, sqlpp::integer);
		object_struct(count_S_ctb, sqlpp::integer);
		object_struct(count_X_ctb, sqlpp::integer);
		object_struct(count_SH_ctb, sqlpp::integer);
		object_struct(count_XH_ctb, sqlpp::integer);
		object_struct(count_A_mania, sqlpp::integer);
		object_struct(count_S_mania, sqlpp::integer);
		object_struct(count_X_mania, sqlpp::integer);
		object_struct(count_SH_mania, sqlpp::integer);
		object_struct(count_XH_mania, sqlpp::integer);
	};

	struct user_badges_objects
	{
		object_struct(id, sqlpp::integer);
		object_struct(user, sqlpp::integer);
		object_struct(badge, sqlpp::varchar);
	};

	struct user_preferences_objects
	{
		object_struct(id, sqlpp::integer);

		object_struct(scoreboard_display_classic, sqlpp::boolean);
		object_struct(scoreboard_display_relax, sqlpp::boolean);
		object_struct(score_overwrite_std, sqlpp::boolean);
		object_struct(score_overwrite_taiko, sqlpp::boolean);
		object_struct(score_overwrite_ctb, sqlpp::boolean);
		object_struct(score_overwrite_mania, sqlpp::boolean);

		object_struct(auto_last_classic, sqlpp::integer);
		object_struct(auto_last_relax, sqlpp::integer);
	};

	database_table(users,
		users_objects::id,
		users_objects::username,
		users_objects::safe_username,
		users_objects::registration_date,
		users_objects::latest_activity,
		users_objects::country,
		users_objects::password_md5,
		users_objects::salt,
		users_objects::email,
		users_objects::ip,
		users_objects::roles,
		users_objects::userpage,
		users_objects::background,
		users_objects::status,
		users_objects::favourite_mode,
		users_objects::favourite_relax,
		users_objects::play_style,
		users_objects::is_public,
		users_objects::is_relax
	);

	database_table(user_badges,
		user_badges_objects::id,
		user_badges_objects::user,
		user_badges_objects::badge
	);

	database_table(users_preferences,
		user_preferences_objects::id,
		user_preferences_objects::scoreboard_display_classic,
		user_preferences_objects::scoreboard_display_relax,
		user_preferences_objects::auto_last_classic,
		user_preferences_objects::auto_last_relax,
		user_preferences_objects::score_overwrite_std,
		user_preferences_objects::score_overwrite_taiko,
		user_preferences_objects::score_overwrite_ctb,
		user_preferences_objects::score_overwrite_mania
	);

	database_table(users_stats,
		users_stats_objects::id,
		users_stats_objects::rank_std,
		users_stats_objects::rank_taiko,
		users_stats_objects::rank_ctb,
		users_stats_objects::rank_mania,
		users_stats_objects::ranked_score_std,
		users_stats_objects::total_score_std,
		users_stats_objects::playcount_std,
		users_stats_objects::replays_watched_std,
		users_stats_objects::total_hits_std,
		users_stats_objects::max_combo_std,
		users_stats_objects::level_std,
		users_stats_objects::playtime_std,
		users_stats_objects::avg_accuracy_std,
		users_stats_objects::pp_std,
		users_stats_objects::count_A_std,
		users_stats_objects::count_S_std,
		users_stats_objects::count_SH_std,
		users_stats_objects::count_X_std,
		users_stats_objects::count_XH_std,
		users_stats_objects::ranked_score_taiko,
		users_stats_objects::total_score_taiko,
		users_stats_objects::playcount_taiko,
		users_stats_objects::replays_watched_taiko,
		users_stats_objects::total_hits_taiko,
		users_stats_objects::max_combo_taiko,
		users_stats_objects::level_taiko,
		users_stats_objects::playtime_taiko,
		users_stats_objects::avg_accuracy_taiko,
		users_stats_objects::pp_taiko,
		users_stats_objects::count_A_taiko,
		users_stats_objects::count_S_taiko,
		users_stats_objects::count_SH_taiko,
		users_stats_objects::count_X_taiko,
		users_stats_objects::count_XH_taiko,
		users_stats_objects::ranked_score_ctb,
		users_stats_objects::total_score_ctb,
		users_stats_objects::playcount_ctb,
		users_stats_objects::replays_watched_ctb,
		users_stats_objects::total_hits_ctb,
		users_stats_objects::max_combo_ctb,
		users_stats_objects::level_ctb,
		users_stats_objects::playtime_ctb,
		users_stats_objects::avg_accuracy_ctb,
		users_stats_objects::pp_ctb,
		users_stats_objects::count_A_ctb,
		users_stats_objects::count_S_ctb,
		users_stats_objects::count_SH_ctb,
		users_stats_objects::count_X_ctb,
		users_stats_objects::count_XH_ctb,
		users_stats_objects::ranked_score_mania,
		users_stats_objects::total_score_mania,
		users_stats_objects::playcount_mania,
		users_stats_objects::replays_watched_mania,
		users_stats_objects::total_hits_mania,
		users_stats_objects::max_combo_mania,
		users_stats_objects::level_mania,
		users_stats_objects::playtime_mania,
		users_stats_objects::avg_accuracy_mania,
		users_stats_objects::pp_mania,
		users_stats_objects::count_A_mania,
		users_stats_objects::count_S_mania,
		users_stats_objects::count_SH_mania,
		users_stats_objects::count_X_mania,
		users_stats_objects::count_XH_mania
	);

	database_table(users_stats_relax,
		users_stats_objects::id,
		users_stats_objects::rank_std,
		users_stats_objects::rank_taiko,
		users_stats_objects::rank_ctb,
		users_stats_objects::rank_mania,
		users_stats_objects::ranked_score_std,
		users_stats_objects::total_score_std,
		users_stats_objects::playcount_std,
		users_stats_objects::replays_watched_std,
		users_stats_objects::total_hits_std,
		users_stats_objects::max_combo_std,
		users_stats_objects::level_std,
		users_stats_objects::playtime_std,
		users_stats_objects::avg_accuracy_std,
		users_stats_objects::pp_std,
		users_stats_objects::count_A_std,
		users_stats_objects::count_S_std,
		users_stats_objects::count_SH_std,
		users_stats_objects::count_X_std,
		users_stats_objects::count_XH_std,
		users_stats_objects::ranked_score_taiko,
		users_stats_objects::total_score_taiko,
		users_stats_objects::playcount_taiko,
		users_stats_objects::replays_watched_taiko,
		users_stats_objects::total_hits_taiko,
		users_stats_objects::max_combo_taiko,
		users_stats_objects::level_taiko,
		users_stats_objects::playtime_taiko,
		users_stats_objects::avg_accuracy_taiko,
		users_stats_objects::pp_taiko,
		users_stats_objects::count_A_taiko,
		users_stats_objects::count_S_taiko,
		users_stats_objects::count_SH_taiko,
		users_stats_objects::count_X_taiko,
		users_stats_objects::count_XH_taiko,
		users_stats_objects::ranked_score_ctb,
		users_stats_objects::total_score_ctb,
		users_stats_objects::playcount_ctb,
		users_stats_objects::replays_watched_ctb,
		users_stats_objects::total_hits_ctb,
		users_stats_objects::max_combo_ctb,
		users_stats_objects::level_ctb,
		users_stats_objects::playtime_ctb,
		users_stats_objects::avg_accuracy_ctb,
		users_stats_objects::pp_ctb,
		users_stats_objects::count_A_ctb,
		users_stats_objects::count_S_ctb,
		users_stats_objects::count_SH_ctb,
		users_stats_objects::count_X_ctb,
		users_stats_objects::count_XH_ctb
	);

}

#endif