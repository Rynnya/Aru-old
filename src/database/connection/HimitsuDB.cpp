#include "HimitsuDB.h"
#include "oatpp/core/macro/component.hpp"

himitsu::Connection::Connection()
{
	OATPP_COMPONENT(std::shared_ptr<sqlpp::mysql::connection_config>, config);
	conn = std::make_unique<sqlpp::mysql::connection>(config);
}

himitsu::Connection::~Connection()
{
	ConnectionPool::releaseConnection();
}

sqlpp::mysql::connection& himitsu::Connection::operator*()
{
	return (*conn);
}

sqlpp::mysql::connection* himitsu::Connection::operator->()
{
	return conn.get();
}

void himitsu::ConnectionPool::releaseConnection()
{
	std::lock_guard<std::mutex> _lock(_mtx);
	connection_amount--;
}

himitsu::Connection himitsu::ConnectionPool::createConnection()
{
	std::lock_guard<std::mutex> _lock(_mtx);

	while (true)
	{
		if (connection_amount < 10)
		{
			connection_amount++;
			return himitsu::Connection();
		}

		_lock.~lock_guard();
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		continue;
	}
}
