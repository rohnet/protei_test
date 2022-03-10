#ifndef PROTEI_TEST_TASK_BIND_POLICY_H
#define PROTEI_TEST_TASK_BIND_POLICY_H

#include <proto.h>
#include <in_address.h>
#include <socket_states/active_socket.h>
#include <socket_states/binded_socket.h>

#include <type_traits>

namespace protei::sock::policies
{

template <template <typename> typename D, typename Proto, typename = void>
struct bind_policy
{
public:
    std::optional<binded_socket_t<Proto>> bind(in_address_port_t const& local) noexcept
    {
        auto& der = derived();
        if (der.m_impl.bind(local))
        {
            return binded_socket_t<Proto>{local, std::move(der.m_impl)};
        }
        else
        {
            return std::nullopt;
        }
    }
private:
    D<Proto>& derived() noexcept
    {
        static_assert(std::is_base_of_v<bind_policy, D<Proto>>);
        return static_cast<D<Proto>&>(*this);
    }
};


template <template <typename> typename D, typename Proto>
struct bind_policy<D, Proto, is_datagram_t<Proto>>
{
public:
    std::optional<active_socket_t<Proto>> bind(in_address_port_t const& local) noexcept
    {
        auto& der = derived();
        if (der.m_impl.bind(local))
        {
            return active_socket_t<Proto>{std::move(der.m_impl), std::nullopt, local};
        }
        else
        {
            return std::nullopt;
        }
    }
private:
    D<Proto>& derived() noexcept
    {
        static_assert(std::is_base_of_v<bind_policy, D<Proto>>);
        return static_cast<D<Proto>&>(*this);
    }
};

}

#endif //PROTEI_TEST_TASK_BIND_POLICY_H