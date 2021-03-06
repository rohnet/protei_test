#ifndef PROTEI_TEST_TASK_ADDRESS_FROM_STRING_H
#define PROTEI_TEST_TASK_ADDRESS_FROM_STRING_H

#include <socket/in_address.h>

namespace protei::utils
{

/**
 * @brief Create address with port from string
 * @param str - address string
 * @param port - port
 * @return created address if string was valid
 */
std::optional<sock::in_address_port_t> from_string_and_port(std::string const& str, std::uint_fast16_t port) noexcept;

}

#endif //PROTEI_TEST_TASK_ADDRESS_FROM_STRING_H
