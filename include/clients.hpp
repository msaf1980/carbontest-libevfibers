#ifndef _CLIENTS_HPP_
#define _CLIENTS_HPP_

#include <cstdlib>

#include <config.hpp>
#include <netstat.hpp>

#define MAX_MESSAGE_LEN 1024

void clientThread(const int &thread_id, const int &thread_count, const Config &config, NetStatQueue &queue);

#endif /* _CLIENTS_HPP_ */
