#include <endpoint/client.h>
#include <socket/af_inet.h>
#include <epoll/epoll.h>

#include "async_stdin.h"

#include <iostream>
#include <csignal>
#include <thread>

using namespace protei;
using namespace protei::sock;
using namespace protei::epoll;
using namespace protei::utils;
using namespace protei::endpoint;

static sig_atomic_t volatile main_loop = 1;

void sig(int) noexcept
{
    if (!main_loop)
    {
        exit(1);
    }
    std::cout << "Stop App" << std::endl;
    main_loop = 0;
}


template <typename Client>
bool init(Client& cl, int port, std::optional<int> local_port) noexcept
{
    return (local_port ? cl->start("127.0.0.1", *local_port) : cl->start())
           && cl->connect(
            "127.0.0.1"
            , port
            , [](){ }
            , [&cl]()
            {
                std::string resp;
                do
                {
                    std::string tmp_buff;
                    tmp_buff.resize(256);
                    auto rec = cl->recv(tmp_buff.data(), tmp_buff.size());
                    if (rec)
                    {
                        resp += tmp_buff.substr(0, rec->second);
                    }
                } while (!cl->finished_recv());
                std::cout << "response: " << resp << std::endl;
            }
            , [](){ std::cout << "disconnect" << std::endl; });
}


int main(int argc, char* argv[])
{
    if (argc < 3) { std::terminate(); }
    int remote_port = std::stoi(argv[2]);
    std::optional<int> local_port;
    if (argc > 3)
    {
        local_port = std::stoi(argv[3]);
    }
    std::string proto = argv[1];
    std::optional<protei::endpoint::client_t<tcp, epoll_t>> client_tcp;
    std::optional<protei::endpoint::client_t<udp, epoll_t>> client_udp;
    client_i* client = nullptr;
    if (proto == "tcp")
    {
        client_tcp.emplace(epoll_t{5, 10u}, ipv4{});
        if (init(client_tcp, remote_port, std::nullopt))
        {
            client = &*client_tcp;
        }
    }
    else
    {
        client_udp.emplace(epoll_t{5, 10u}, ipv4{});
        if (init(client_udp, remote_port, local_port))
        {
            client = &*client_udp;
        }
    }

    if (!client)
    {
        std::cerr << "error initializing client";
        std::terminate();
    }

    std::signal(SIGTERM, sig);
    std::signal(SIGINT, sig);

    async_stdin ai;
    while (main_loop)
    {
        client->proceed(std::chrono::milliseconds{10});
        if (auto input = ai.read_line())
        {
            auto sent = client->send(input->data(), input->size());
            if (!sent && !client->finished_send() && !client->finished_recv())
            {
                std::cerr << "error sending request {" << *input << "}" << std::endl;
            }
        }
        else
        {
            std::this_thread::yield();
        }
    }

    return 0;
}
