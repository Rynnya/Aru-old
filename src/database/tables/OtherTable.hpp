#ifndef OtherTable_hpp
#define OtherTable_hpp

#include "CoreTable.hpp"

struct token_objects
{
	object_struct(id,           sqlpp::integer);
	object_struct(user,         sqlpp::integer);
	object_struct(privileges,   sqlpp::integer);
	object_struct(token,        sqlpp::varchar);
	object_struct(t_private,    sqlpp::boolean);
	object_struct(last_updated, sqlpp::bigint);
};

database_table(tokens,
	token_objects::id,
	token_objects::user,
	token_objects::privileges,
	token_objects::token,
	token_objects::t_private,
	token_objects::last_updated
);

#endif
