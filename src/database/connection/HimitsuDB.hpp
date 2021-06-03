#ifndef database_connection_HimitsuDB_hpp_included
#define database_connection_HimitsuDB_hpp_included

#include "Globals.hpp"
#include <thread>
#include <mutex>
#include <list>

namespace himitsu
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
		ConnectionPool(int size);
		~ConnectionPool();
		inline himitsu::Connection getConnection();
	private:
		static ConnectionPool* inst_;
		inline void returnConnection(Connection&& connection);
		int size;
		std::mutex _mtx;
		std::condition_variable _wait;
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
		himitsu::ConnectionPool::getInstance()->returnConnection(std::move(*this));
	}

	sqlpp::mysql::connection& Connection::operator*()
	{
		return *connection;
	}

	sqlpp::mysql::connection* Connection::operator->()
	{
		return connection;
	}

	himitsu::ConnectionPool* ConnectionPool::getInstance()
	{
		return inst_;
	}

	himitsu::Connection ConnectionPool::getConnection()
	{
		while (true)
		{
			std::unique_lock lock(_mtx);
			if (m_connections.empty())
			{
				lock.unlock();
				std::this_thread::sleep_for(std::chrono::microseconds(5));
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
