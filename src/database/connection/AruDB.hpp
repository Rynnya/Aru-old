#ifndef database_connection_AruDB_hpp_included
#define database_connection_AruDB_hpp_included

#include "Globals.hpp"
#include <thread>
#include <mutex>
#include <list>

namespace aru
{
	class ConnectionPool;

	class Connection final
	{
		friend ConnectionPool;
	public:
		inline Connection(const Connection& other);
		inline Connection(Connection&& other) noexcept;
		Connection& operator=(const Connection&) = delete;
		Connection& operator=(std::nullptr_t);
		inline ~Connection();
		inline sqlpp::mysql::connection& operator*();
		inline sqlpp::mysql::connection* operator->();
		template <typename T>
		inline auto operator()(const T& t) { return this->connection->run(t); }
	private:
		inline Connection(std::shared_ptr<sqlpp::mysql::connection_config>& connection);
		bool end_of_life = false;
		sqlpp::mysql::connection* connection = nullptr;
	};

	class ConnectionPool
	{
		friend Connection;
	public:
		inline static ConnectionPool* getInstance();
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

	Connection::Connection(std::shared_ptr<sqlpp::mysql::connection_config>& config)
	{
		this->connection = new sqlpp::mysql::connection(config);
	}

	Connection::Connection(const Connection& other)
	{
		if (&other == this)
			return;
		this->connection = other.connection;
	}

	Connection::Connection(Connection&& other) noexcept
	{
		if (&other == this)
			return;
		this->connection = other.connection;
		other.connection = nullptr;
	}

	Connection::~Connection()
	{
		if (end_of_life) return;
		if (!this->connection) return;
		aru::ConnectionPool::getInstance()->returnConnection(std::move(*this));
	}

	sqlpp::mysql::connection& Connection::operator*()
	{
		return *connection;
	}

	sqlpp::mysql::connection* Connection::operator->()
	{
		return connection;
	}

	aru::ConnectionPool* ConnectionPool::getInstance()
	{
		return inst_;
	}

	aru::Connection ConnectionPool::getConnection()
	{
		while (true)
		{
			std::unique_lock lock(_mtx);
			if (m_connections.empty())
			{
				lock.unlock();
				std::this_thread::sleep_for(std::chrono::milliseconds(5));
				continue;
			}

			Connection sp = std::move(m_connections.front());
			m_connections.pop_front();
			return sp;
		}
	}

	void ConnectionPool::returnConnection(Connection&& connection)
	{
		std::unique_lock lock(_mtx);
		m_connections.push_back(std::move(connection));
	}
}


#endif