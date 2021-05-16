#ifndef HimitsuDB_hpp
#define HimitsuDB_hpp

#include "Globals.hpp"

namespace himitsu
{
	class Connection
	{
	public:
		Connection(std::unique_ptr<sqlpp::mysql::connection> connection);
		~Connection();
		void destroyConnection();
		sqlpp::mysql::connection& operator*();
		sqlpp::mysql::connection* operator->();
	private:
		bool end_of_life = false;
		std::unique_ptr<sqlpp::mysql::connection> connection = nullptr;
	};

	class ConnectionPool
	{
		friend Connection;
	public:
		static ConnectionPool* getInstance();
		ConnectionPool(int size);
		~ConnectionPool();
		std::shared_ptr<himitsu::Connection> getConnection();
	private:
		static ConnectionPool* inst_;
		void returnConnection(std::unique_ptr<sqlpp::mysql::connection> connection);
		int size;
		std::mutex _mtx;
		std::list<std::shared_ptr<Connection>> m_connections;
	};
}

#endif