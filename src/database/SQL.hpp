#ifndef database_SQL_hpp_included
#define database_SQL_hpp_included

#include "Globals.hpp"
#include <mutex>
#include <list>

namespace aru
{
	class ConnectionPool;

	class Connection final
	{
		friend ConnectionPool;
	public:
		inline Connection() {};
		inline Connection(const Connection& other);
		inline Connection(Connection&& other) noexcept;
		Connection& operator=(const Connection&) = delete;
		Connection& operator=(std::nullptr_t);
		inline bool operator==(std::nullptr_t) { return connection == nullptr; };
		inline ~Connection();
		sqlpp::mysql::connection& operator*() const;
		sqlpp::mysql::connection* operator->() const;
		template <typename T>
		inline auto operator()(const T& t) const { return this->connection->run(t); }
	private:
		inline Connection(std::shared_ptr<sqlpp::mysql::connection_config>& connection);
		bool end_of_life = false;
		sqlpp::mysql::connection* connection = nullptr;
	};

	inline Connection::Connection(const Connection& other)
	{
		if (&other == this)
			return;

		this->connection = other.connection;
	}

	inline Connection::Connection(Connection&& other) noexcept
	{
		if (&other == this)
			return;

		this->connection = std::move(other.connection);
		other.connection = nullptr;
	}

	class ConnectionPool
	{
		friend Connection;
	public:
		inline static ConnectionPool* getInstance() { return inst_; };
		ConnectionPool(int32_t size);
		~ConnectionPool();
		inline aru::Connection getConnection();
		void wakeUpPool();
	private:
		static ConnectionPool* inst_;
		inline void returnConnection(Connection&& connection);
		int32_t size;
		std::mutex _mtx;
		std::list<Connection> m_connections;
	};

	inline Connection ConnectionPool::getConnection()
	{
		std::unique_lock lock(_mtx);
		if (m_connections.empty())
			return aru::Connection();

		Connection sp = std::move(m_connections.front());
		m_connections.pop_front();
		return sp;
	}

	inline void ConnectionPool::returnConnection(Connection&& connection)
	{
		std::unique_lock lock(_mtx);
		m_connections.push_back(std::move(connection));
	}
}

class SQLHelper
{
public:
	static std::pair<uint32_t, uint32_t> Paginate(int32_t page, int32_t length, int32_t max)
	{
		if (page < 1)
			page = 1;

		if (length < 1)
			length = 1;

		length = std::max(length, max);

		uint32_t start_pos = (page - 1) * length;
		return std::make_pair(start_pos, length);
	}
};

#endif
