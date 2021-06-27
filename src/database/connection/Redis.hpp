#ifndef database_connection_Redis_hpp_included
#define database_connection_Redis_hpp_included

#include "Globals.hpp"
#include "cpp_redis/core/client.hpp"

namespace aru
{
	class redis
	{
	private:
		std::shared_ptr<cpp_redis::client> client = nullptr;
        std::string address;
        size_t port;
        std::string password;
        uint32_t attempts;
        uint32_t current_attempt;

    public:
        redis(std::string address, size_t port, uint32_t attempts = 3, std::string password = "");

        void connect();
        void disconnect();

		/*
		* Returns Z set value as long long from Redis
		* If null, returns 0 as long long
		*/
		long long getRedisRank(std::string key, std::string id)
		{
			int32_t temp = 0;
			this->client->zrevrank(key, id, [&](cpp_redis::reply& reply)
				{
					if (reply)
						temp = reply.as_integer() + 1;
				});
			this->client->sync_commit();
			return temp;
		}

		/*
		* Returns string value from Redis
		* If null, returns -1 as string
		*/
		std::string getRedisString(std::string key)
		{
			std::string temp = "-1";
			this->client->get(key, [&](cpp_redis::reply& reply)
				{
					if (reply)
						temp = reply.as_string();
				});
			this->client->sync_commit();
			return temp;
		}

        bool is_connected();
        std::shared_ptr<cpp_redis::client> get();
	};
}

#endif
