#ifndef database_sql_hpp_included
#define database_sql_hpp_included

#include "globals.hpp"
#include <mutex>
#include <list>

namespace aru
{
	class connection_pool;

	class database final
	{
		friend connection_pool;
	public:
		database() {};
		database(const database& other);
		database(database&& other) noexcept;
		database& operator=(const database&) = delete;
		database& operator=(std::nullptr_t);
		inline bool operator==(std::nullptr_t) { return connection == nullptr; };
		inline ~database();
		sqlpp::mysql::connection& operator*() const;
		sqlpp::mysql::connection* operator->() const;
		template <typename T>
		inline auto operator()(const T& t) const { return this->connection->run(t); }
	private:
		database(std::shared_ptr<sqlpp::mysql::connection_config>& connection);
		bool end_of_life = false;
		sqlpp::mysql::connection* connection = nullptr;
	};

	class connection_pool
	{
		friend database;
	public:
		inline static connection_pool* get_instance() { return inst_; };
		connection_pool(int32_t size);
		~connection_pool();
		aru::database get_connection();
		void wakeUpPool();
	private:
		static connection_pool* inst_;
		void return_connection(database&& connection);
		int32_t size;
		std::mutex _mtx;
		std::list<database> m_connections;
	};
}

class sql_helper
{
public:
	static std::pair<uint32_t, uint32_t> paginate(int32_t page, int32_t length, int32_t max);
};

#endif
