#ifndef PROTEI_TEST_TASK_ACCEPT_POLICY_H
#define PROTEI_TEST_TASK_ACCEPT_POLICY_H

#include <socket/proto.h>
#include <socket/in_address.h>
#include <utils/mbind.h>
#include <socket_states/active_socket.h>

#include <type_traits>

namespace protei::sock::policies
{

/**
 * @brief Accept policy for connection based protocols
 * @tparam D - derived type
 * @tparam Proto - protocol type
 */
template <template <typename> typename D, typename Proto, typename = void>
struct accept_policy
{
public:
    std::optional<active_socket_t<Proto>> accept() const
    {
        using utils::mbind;
        return mbind(
                derived().m_impl.accept()
                , [this](std::pair<protei::sock::impl::socket_impl, in_address_port_t>&& accepted)
                        -> std::optional<active_socket_t<Proto>>
                {
                    return active_socket_t<Proto>{std::move(accepted.first), accepted.second, derived().local(), true};
                });
    }
private:
    D<Proto> const& derived() const noexcept
    {
        static_assert(std::is_base_of_v<accept_policy, D<Proto>>);
        return static_cast<D<Proto> const&>(*this);
    }
};


/**
 * @brief Accept policy for connectionless protocols
 * @tparam D - derived type
 * @tparam Proto - protocol type
 */
template <template <typename> typename D, typename Proto>
struct accept_policy<D, Proto, is_connectionless_t<Proto>>
{};

}

#endif //PROTEI_TEST_TASK_ACCEPT_POLICY_H
