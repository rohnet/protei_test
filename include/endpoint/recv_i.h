#ifndef PROTEI_TEST_TASK_RECV_I_H
#define PROTEI_TEST_TASK_RECV_I_H

#include <cstdint>
#include <optional>

namespace protei::sock
{
struct in_address_port_t;
}

namespace protei::endpoint
{

class recv_i
{
public:
    virtual ~recv_i() = default;
    std::optional<std::pair<sock::in_address_port_t, std::size_t>> recv(void* buffer, std::size_t buff_size);
    bool finished_recv() const;
private:
    virtual std::optional<std::pair<sock::in_address_port_t, std::size_t>>
            recv_impl(void* buffer, std::size_t buff_size) = 0;
    virtual bool finished_recv_impl() const = 0;
};

}

#endif //PROTEI_TEST_TASK_RECV_I_H
