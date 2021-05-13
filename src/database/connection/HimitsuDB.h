#ifndef HIMITSU_DB
#define HIMITSU_DB

#include "Globals.h"

namespace himitsu
{
	class Connection
	{
	public:
		Connection();
		~Connection();
		sqlpp::mysql::connection& operator*();
		sqlpp::mysql::connection* operator->();
	private:
		std::unique_ptr<sqlpp::mysql::connection> conn = nullptr;
	};

	// TODO: Rewrite this mess to non-static, so releaseConnection cannot be used outside of ~Connection
	class ConnectionPool
	{
	public:
		static void releaseConnection();
		static himitsu::Connection createConnection();
	private:
		inline static int connection_amount = 0;
		inline static std::mutex _mtx = std::mutex();
	};
}

#endif // !HIMITSU_DB
