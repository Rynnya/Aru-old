#ifndef handlers_PoolHandler_hpp_included
#define handlers_PoolHandler_hpp_included

#include "../database/SQL.hpp"
#include "oatpp/core/async/Coroutine.hpp"

class PoolHandler : public oatpp::async::CoroutineWithResult<PoolHandler, const aru::Connection&>
{
	Action act() override
	{
		auto& connection = aru::ConnectionPool::getInstance()->getConnection();
		if (connection == nullptr)
			return waitRepeat(std::chrono::milliseconds(5));

		return _return(std::move(connection));
	}
};

#endif