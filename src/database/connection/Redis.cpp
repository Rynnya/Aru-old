#include "Redis.hpp"

aru::redis::redis(std::string address, size_t port, uint32_t attempts, std::string password)
    : address(std::move(address))
    , port(port)
    , attempts(attempts)
    , current_attempt(0)
    , password(std::move(password)) {
    // Initialized in initializer list
}

void aru::redis::connect() {
    if (is_connected())
        return;

    this->client = std::make_shared<cpp_redis::client>();
    this->client->connect(this->address, this->port);

    if (!this->password.empty())
        this->client->auth(this->password);

    if (!is_connected() && current_attempt <= attempts)
    {
        current_attempt++;
        std::this_thread::sleep_for(std::chrono::seconds(3));
        connect();
    }

    if (!is_connected())
    {
        std::cout << "Cannot connect to Redis. Server is down?" << std::endl;
        std::cout << "Press any button to exit." << std::endl;
#pragma warning(disable : 6031)
        getchar();
        exit(-1);
    }
}

void aru::redis::disconnect() {
    std::chrono::seconds seconds = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()
    );

    if (!is_connected())
        return;

    this->client->disconnect();
}

bool aru::redis::is_connected() {
    return this->client != nullptr;
}

std::shared_ptr<cpp_redis::client> aru::redis::get() {
    if (!is_connected())
    {
        connect();
    }

    return this->client;
}