#ifndef HIMITSU_SQL
#define HIMITSU_SQL

#include "Globals.h"

class SQLHelper
{
public:
	static std::pair<unsigned int, unsigned int> Paginate(int page, int length, int max)
	{
		if (page < 1) page = 1;
		if (length < 1) length = 1;
		if (length > max) length = max;
		unsigned int start_pos = (page - 1) * length;
		return std::make_pair(start_pos, length);
	}

	static int Limitize(int start, int number, int end)
	{
		if (number < start)
			return start;
		if (number > end)
			return end;
		return number;
	}
};

#endif // !HIMITSU_SQL
