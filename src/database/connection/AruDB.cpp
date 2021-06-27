#include "AruDB.hpp"
#include "oatpp/core/macro/component.hpp"

aru::ConnectionPool* aru::ConnectionPool::inst_;

aru::Connection& aru::Connection::operator=(std::nullptr_t)
{
	this->end_of_life = true;
	delete this->connection;
	this->connection = nullptr;
	return *this;
}

aru::ConnectionPool::ConnectionPool(int32_t size)
{
	OATPP_COMPONENT(std::shared_ptr<sqlpp::mysql::connection_config>, config);
	for (int32_t i = 0; i < size; i++)
	{
		m_connections.push_back(aru::Connection(config));
	}
	this->size = size;
	inst_ = this;
}

aru::ConnectionPool::~ConnectionPool()
{
	for (auto& connection : m_connections)
	{
		connection = nullptr;
	}
}

void aru::ConnectionPool::wakeUpPool()
{
	std::unique_lock lock(_mtx);
	for (auto& connection : m_connections)
	{
		if (connection.end_of_life)
			continue;

		const tables::tokens tokens_table {};
		(*connection)(sqlpp::select(tokens_table.id).from(tokens_table).where(tokens_table.id == 0));
	}
}
