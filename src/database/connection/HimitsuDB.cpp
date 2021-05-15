#include "HimitsuDB.h"
#include "oatpp/core/macro/component.hpp"

himitsu::ConnectionPool* himitsu::ConnectionPool::inst_;

himitsu::Connection::Connection(std::unique_ptr<sqlpp::mysql::connection> connection)
{
	this->connection = std::move(connection);
}

himitsu::Connection::~Connection()
{
	if (end_of_life) return;
	himitsu::ConnectionPool::getInstance()->returnConnection(std::move(this->connection));
}

void himitsu::Connection::destroyConnection()
{
	this->end_of_life = true;
	this->connection.release();
}

sqlpp::mysql::connection& himitsu::Connection::operator*()
{
	return (*connection);
}

sqlpp::mysql::connection* himitsu::Connection::operator->()
{
	return connection.get();
}

himitsu::ConnectionPool* himitsu::ConnectionPool::getInstance()
{
	return inst_;
}

himitsu::ConnectionPool::ConnectionPool(int size)
{
	OATPP_COMPONENT(std::shared_ptr<mysql::connection_config>, config);
	for (int i = 0; i < size; i++)
	{
		auto mysql_conn = std::make_unique<sqlpp::mysql::connection>(config);
		m_connections.push_back(std::make_shared<himitsu::Connection>(std::move(mysql_conn)));
	}
	this->size = size;
	inst_ = this;
}

himitsu::ConnectionPool::~ConnectionPool()
{
	for (int i = 0; i < size; i++)
	{
		std::shared_ptr<Connection> sp = m_connections.front();
		m_connections.pop_front();
		sp->destroyConnection();
	}
}

std::shared_ptr<himitsu::Connection> himitsu::ConnectionPool::getConnection()
{
	std::unique_lock lock(_mtx);
	while (true)
	{
		if (m_connections.empty())
		{
			lock.unlock();
			std::this_thread::sleep_for(std::chrono::milliseconds(5));
			continue;
		}

		std::shared_ptr<Connection> sp = m_connections.front();
		m_connections.pop_front();
		return sp;
	}
}

void himitsu::ConnectionPool::returnConnection(std::unique_ptr<sqlpp::mysql::connection> connection)
{
	std::unique_lock lock(_mtx);
	m_connections.push_back(std::make_shared<himitsu::Connection>(std::move(connection)));
}