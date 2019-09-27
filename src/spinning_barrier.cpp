#include <spinning_barrier.hpp>

SpinningBarrier::SpinningBarrier(unsigned int count)
    : count(count), isNotWaiting(false), waitCount(0) {}

bool SpinningBarrier::wait() {
	if (waitCount.fetch_add(1) >= count - 1) {
		/* barrier reached */
		isNotWaiting = true;

		condVar.notify_all();
		waitCount.store(0);
		return true;
	} else {
		std::unique_lock<std::mutex> lock(mutex);

		condVar.wait(lock, [&] { return isNotWaiting; });
		return false;
	}
}
