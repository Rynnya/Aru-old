#ifndef database_SQL_hpp_included
#define database_SQL_hpp_included

#include "Globals.hpp"

#include "tables/OtherTable.hpp"

class SQLHelper
{
public:
	static std::pair<uint32_t, uint32_t> Paginate(int32_t page, int32_t length, int32_t max)
	{
		if (page < 1)
			page = 1;

		if (length < 1)
			length = 1;

		if (length > max)
			length = max;

		uint32_t start_pos = (page - 1) * length;
		return std::make_pair(start_pos, length);
	}
};

#endif
