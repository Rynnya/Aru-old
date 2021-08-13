#include "sql.hpp"
#include "oatpp/core/macro/component.hpp"
#include "tables/other_table.hpp"

aru::connection_pool* aru::connection_pool::inst_;

aru::database::database(const database& other)
{
	if (&other == this)
	{
		return;
	}

	this->connection = other.connection;
}

aru::database::database(database&& other) noexcept
{
	if (&other == this)
	{
		return;
	}

	this->connection = std::move(other.connection);
	other.connection = nullptr;
}

aru::database::~database()
{
	if (end_of_life || !this->connection)
	{
		return;
	}

	aru::connection_pool::get_instance()->return_connection(std::move(*this));
}

aru::database& aru::database::operator=(std::nullptr_t)
{
	this->end_of_life = true;
	delete this->connection;
	this->connection = nullptr;
	return *this;
}

sqlpp::mysql::connection& aru::database::operator*() const
{
	return *connection;
}

sqlpp::mysql::connection* aru::database::operator->() const
{
	return connection;
}

aru::database::database(std::shared_ptr<sqlpp::mysql::connection_config>& connection)
{
	this->connection = new sqlpp::mysql::connection(connection);
}

aru::connection_pool::connection_pool(int32_t size)
{
	OATPP_COMPONENT(std::shared_ptr<sqlpp::mysql::connection_config>, config);
	for (int32_t i = 0; i < size; i++)
	{
		m_connections.push_back(std::move(aru::database(config)));
	}

	this->size = size;
	inst_ = this;
}

aru::connection_pool::~connection_pool()
{
	for (auto& connection : m_connections)
	{
		connection = nullptr;
	}
}

aru::database aru::connection_pool::get_connection()
{
	std::unique_lock lock(_mtx);
	if (m_connections.empty())
	{
		return aru::database();
	}

	database sp = std::move(m_connections.front());
	m_connections.pop_front();
	return sp;
}

void aru::connection_pool::wakeUpPool()
{
	std::unique_lock lock(_mtx);
	for (auto& connection : m_connections)
	{
		if (connection == nullptr)
		{
			continue;
		}

		const tables::tokens tokens_table{};
		(*connection)(sqlpp::select(tokens_table.id).from(tokens_table).where(tokens_table.id == 0));
	}
}

void aru::connection_pool::return_connection(database&& connection)
{
	std::unique_lock lock(_mtx);
	m_connections.push_back(std::move(connection));
}

std::pair<uint32_t, uint32_t> sql_helper::paginate(int32_t page, int32_t length, int32_t max)
{
	if (page < 1)
	{
		page = 1;
	}

	aru::utils::sanitize(length, 1, max, 50);

	uint32_t start_pos = (page - 1) * length;
	return std::make_pair(start_pos, length);
}