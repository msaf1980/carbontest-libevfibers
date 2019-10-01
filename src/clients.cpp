#include <stdlib.h>

#include <vector>

#include <plog/Log.h>

#include <ev.h>
#include <evfibers/fiber.h>

#include <clients.hpp>

using std::vector;

static void TCPClient(struct fbr_context *fctx, void *_arg) {
	LOG_VERBOSE << "Starting TCP client";
}

static void UDPClient(struct fbr_context *fctx, void *_arg) {
	LOG_VERBOSE << "Starting TCP client";
}

void clientThread(const int &thread_id, const int &thread_count,
                  const Config &config, NetStatQueue &queue) {
	LOG_VERBOSE << "Starting client thread " << thread_id;

	struct ev_loop *loop = ev_loop_new(0);
	struct fbr_context fbr;
	vector<fbr_id_t> fbr_ids;
	int pos = 0;

	fbr_init(&fbr, loop);

	// ticker_id = fbr_create(&fbr, "ticker", ticker, NULL, 0);

	for (int i = thread_id; i < config.Workers; i += thread_count) {
		LOG_VERBOSE << "Starting TCP client " << i << " in client thread "
		            << thread_id;
		fbr_ids.push_back(fbr_create(&fbr, "ticker", TCPClient, NULL, 0));
		if (fbr_id_isnull(fbr_ids[pos])) {
			LOG_FATAL << "Unable to transfer fiber " << pos;
			goto ERROR;
		}
		if (fbr_transfer(&fbr, fbr_ids[pos]) < 0) {
			LOG_FATAL << "Unable to transfer fiber " << pos;
			goto ERROR;
		}
		pos++;
	}
	for (int i = thread_id; i < config.UWorkers; i += thread_count) {
		LOG_VERBOSE << "Starting UDP client " << i << " in client thread "
		            << thread_id;
		fbr_ids.push_back(fbr_create(&fbr, "ticker", UDPClient, NULL, 0));
		if (fbr_id_isnull(fbr_ids[pos])) {
			LOG_FATAL << "Unable to transfer fiber " << pos;
			goto ERROR;
		}
		if (fbr_transfer(&fbr, fbr_ids[pos]) < 0) {
			LOG_FATAL << "Unable to transfer fiber " << pos;
			goto ERROR;
		}
		pos++;
	}

	ev_run(loop, 0);
	return;
ERROR:
	running.store(false);
	return;
}
