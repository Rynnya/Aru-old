#include "SQL.hpp"
#include "oatpp/core/macro/component.hpp"
#include "tables/OtherTable.hpp"

aru::ConnectionPool* aru::ConnectionPool::inst_;

inline aru::Connection::~Connection()
{
	if (end_of_life) return;
	if (!this->connection) return;
	aru::ConnectionPool::getInstance()->returnConnection(std::move(*this));
}

aru::Connection& aru::Connection::operator=(std::nullptr_t)
{
	this->end_of_life = true;
	delete this->connection;
	this->connection = nullptr;
	return *this;
}

sqlpp::mysql::connection& aru::Connection::operator*() const
{
	return *connection;
}

sqlpp::mysql::connection* aru::Connection::operator->() const
{
	return connection;
}

inline aru::Connection::Connection(std::shared_ptr<sqlpp::mysql::connection_config>& connection)
{
	this->connection = new sqlpp::mysql::connection(connection);
}

aru::ConnectionPool::ConnectionPool(int32_t size)
{
	OATPP_COMPONENT(std::shared_ptr<sqlpp::mysql::connection_config>, config);
	for (int32_t i = 0; i < size; i++)
		m_connections.push_back(std::move(aru::Connection(config)));

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
		if (connection == nullptr)
			continue;

		const tables::tokens tokens_table{};
		(*connection)(sqlpp::select(tokens_table.id).from(tokens_table).where(tokens_table.id == 0));
	}
}
