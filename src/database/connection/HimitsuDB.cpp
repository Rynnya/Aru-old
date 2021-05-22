#include "HimitsuDB.hpp"
#include "oatpp/core/macro/component.hpp"

himitsu::ConnectionPool* himitsu::ConnectionPool::inst_;

himitsu::Connection& himitsu::Connection::operator=(std::nullptr_t)
{
	this->end_of_life = true;
	delete this->connection;
	this->connection = nullptr;
	return *this;
}

himitsu::ConnectionPool::ConnectionPool(int size)
{
	OATPP_COMPONENT(std::shared_ptr<sqlpp::mysql::connection_config>, config);
	for (int i = 0; i < size; i++)
	{
		m_connections.push_back(himitsu::Connection(config));
	}
	this->size = size;
	inst_ = this;
}

himitsu::ConnectionPool::~ConnectionPool()
{
	for (auto& connection : m_connections)
	{
		connection = nullptr;
	}
}