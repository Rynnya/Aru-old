#ifndef database_tables_OtherTable_hpp_included
#define database_tables_OtherTable_hpp_included

#include "CoreTable.hpp"

namespace tables
{

	struct token_objects
	{
		object_struct(id, sqlpp::integer);
		object_struct(user, sqlpp::integer);
		object_struct(privileges, sqlpp::integer);
		object_struct(token, sqlpp::varchar);
		object_struct(last_updated, sqlpp::bigint);
		object_detailed_struct("private", _private, sqlpp::boolean);
	};

	database_table(tokens,
		token_objects::id,
		token_objects::user,
		token_objects::privileges,
		token_objects::token,
		token_objects::_private,
		token_objects::last_updated
	);

}

#endif
