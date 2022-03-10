#ifndef PROTEI_TEST_TASK_SEND_RECV_POLICY_H
#define PROTEI_TEST_TASK_SEND_RECV_POLICY_H

#include <proto.h>
#include <in_address.h>

#include <type_traits>

namespace protei::sock::policies
{

template <template <typename> typename D, typename Proto, typename = void>
struct send_recv_policy
{
public:
    std::optional<std::size_t> send(void* buffer, std::size_t size, int flags) noexcept
    {
        return derived().m_impl.send(buffer, size, flags);
    }

    std::optional<std::size_t> receive(void* buffer, std::size_t size, int flags) noexcept
    {
        return derived().m_impl.receive(buffer, size, flags);
    }
private:
    D<Proto>& derived() noexcept
    {
        static_assert(std::is_base_of_v<send_recv_policy, D<Proto>>);
        return static_cast<D<Proto>&>(*this);
    }
};


template <template <typename> typename D, typename Proto>
struct send_recv_policy<D, Proto, is_datagram_t<Proto>>
{
public:
    std::optional<std::size_t> send(in_address_port_t remote, void* buffer, std::size_t size, int flags) noexcept
    {
        return derived().m_impl.send_to(remote, buffer, size, flags);
    }

    std::optional<std::pair<in_address_port_t, std::size_t>> receive(void* buffer, std::size_t size, int flags) noexcept
    {
        return derived().m_impl.receive_from(buffer, size, flags);
    }
private:
    D<Proto>& derived() noexcept
    {
        static_assert(std::is_base_of_v<send_recv_policy, D<Proto>>);
        return static_cast<D<Proto>&>(*this);
    }
};

}

#endif //PROTEI_TEST_TASK_SEND_RECV_POLICY_H
