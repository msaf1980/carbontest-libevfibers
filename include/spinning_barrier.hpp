#ifndef _SPINNING_BARRIER_HPP_
#define _SPINNING_BARRIER_HPP_

#include <atomic>
#include <condition_variable>
#include <mutex>

class SpinningBarrier {
  public:
	SpinningBarrier(unsigned int count);
	SpinningBarrier(const SpinningBarrier &) = delete;
	bool wait();

  private:
	const unsigned int count;
	bool  isNotWaiting;

	std::atomic<unsigned int> waitCount;
	std::condition_variable condVar;
	std::mutex mutex;
};

#endif /* _SPINNING_BARRIER_HPP_ */
