#ifndef handlers_pool_handler_hpp_included
#define handlers_pool_handler_hpp_included

#include "../database/sql.hpp"
#include "oatpp/core/async/Coroutine.hpp"

class pool_handler : public oatpp::async::CoroutineWithResult<pool_handler, const aru::database&>
{
	Action act() override
	{
		auto& connection = aru::connection_pool::get_instance()->get_connection();
		if (connection == nullptr)
		{
			return waitRepeat(std::chrono::milliseconds(5));
		}

		return _return(connection);
	}
};

#endif