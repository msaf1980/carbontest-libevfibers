#include <signal.h>

#include <chrono>
#include <fstream>
#include <iostream>
#include <map>
#include <vector>
#include <thread>

#include <ev.h>

#include <plog/Appenders/ConsoleAppender.h>
#include <plog/Log.h>

#include <fmt/format.h>

#include <spinning_barrier.hpp>

#include <netstat.hpp>
#include <runner.hpp>


using std::map;
using std::string;
using std::vector;
using std::thread;

chrono_clock start, end;
map<string, uint64_t> stat_count;

std::atomic_bool running_queue; // running dequeue flag
SpinningBarrier  queue_wait(2);

int ignore_sigpipe() {
	struct sigaction sa, osa;
	sa.sa_handler = SIG_IGN;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	return sigaction(SIGPIPE, &sa, &osa);
}

void dequeueStat(const Config &config, std::fstream &file,
                 NetStatQueue &queue) {
	NetStat stat;
	while (queue.try_dequeue(stat)) {
		string name = fmt::format("{}.{}.{}", NetProtoStr[stat.Proto],
		                          NetOperStr[stat.Type], NetErrStr[stat.Error]);
		stat_count[name]++;

		file << stat.TimeStamp << "\t" << stat.Id << "\t"
		     << NetProtoStr[stat.Proto] << "\t" << NetOperStr[stat.Type] << "\t"
		     << NetErrStr[stat.Error] << "\t" << stat.Elapsed << "\t"
		     << stat.Size << "\n";

		if (file.fail()) {
			throw std::runtime_error(config.StatFile + " " + strerror(errno));
		}
	}
}

void dequeueThread(const Config &config, NetStatQueue &queue) {
	LOG_VERBOSE << "Starting dequeue thread";
	try {
		std::fstream file;
		file.open(config.StatFile, std::ios_base::in);
		if (file.good()) {
			file.close();
			throw std::runtime_error(config.StatFile + " already exist");
		}
		file.open(config.StatFile, std::ios_base::out);
		if (file.fail()) {
			throw std::runtime_error(config.StatFile + " " + strerror(errno));
		}
		file << "Timestamp\tConId\tProto\tType\tStatus\tElapsed(us)\tSize\n";
		if (file.fail()) {
			throw std::runtime_error(config.StatFile + " " + strerror(errno));
		}

		queue_wait.wait();
		while (running_queue.load()) {
			dequeueStat(config, file, queue);
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}

		file.close();
	} catch (std::exception &e) {
		running.store(false);
		queue_wait.wait();
		// fatal error
		LOG_FATAL << "dequeue thread: " << e.what();
	}
	LOG_VERBOSE << "Shutdown dequeue thread";
}

int runClients(const Config &config) {
	int thread_count = config.Threads - 1;

	static plog::ConsoleAppender<plog::TxtFormatter> consoleAppender;
	plog::init(config.LogLevel, &consoleAppender);

	LOG_INFO << "Starting with " << config.Workers << " TCP clients and "
	         << config.UWorkers << " UDP clients";
	LOG_INFO << "Thread count " << thread_count;

	NetStatQueue queue;

	/* clients in thread */
	vector<size_t> workers_count;
	size_t workers_per_th;
	vector<size_t> uworkers_count;
	size_t uworkers_per_th;

	if (config.Workers > 0) {
		// clientsTCP.resize(config.Workers);
		workers_per_th = config.Workers / thread_count;
		workers_count.resize(thread_count);
		std::fill(workers_count.begin(), workers_count.end(), workers_per_th);
	}
	if (config.UWorkers > 0) {
		// clientsUDP.resize(config.UWorkers);
	}

	running_queue.store(true);
	running.store(true);

	thread *thread_q = new thread(dequeueThread, std::ref(config), std::ref(queue));
	queue_wait.wait();
	if (!running.load())
		return 1;

	// vector<boost::asio::io_context> io_contexts(thread_count);

	//int t = 0;
	//for (int i = 0; i < config.Workers; i++) {
		//if (t == thread_count)
			//t = 0;
	//}
	//for (int i = 0; i < config.UWorkers; i++) {
		//if (t == thread_count)
			//t = 0;
	//}

	start = TIME_NOW;
	std::this_thread::sleep_for(std::chrono::milliseconds(config.Timeout));

	running.store(false);
	end = TIME_NOW;

	LOG_INFO << "Shutting down";

//	for (int i = 0; i < config.Workers; i++) {
//		clientsTCP[i]->stop();
//	}
//	for (int i = 0; i < config.UWorkers; i++) {
//		clientsUDP[i]->stop();
//	}

	// boost::this_thread::sleep_for(boost::chrono::milliseconds(config.Timeout));

	// for (int i = 0; i < thread_count; ++i) {
	// io_contexts[i].stop();
	//}

//	threads_ioc.join_all();
	running_queue.store(false);
	thread_q->join();
	delete thread_q;

	using float_seconds = std::chrono::duration<double>;
	auto duration =
		std::chrono::duration_cast<float_seconds>(end - start).count();
	if (duration > 0) {
		std::cout << std::fixed;
		std::cout << "Test duration " << duration << " s" << std::endl;
		for (auto &it : stat_count) {
			std::cout << it.first << ": " << it.second << " ("
					  << it.second / duration << " op/s)" << std::endl;
		}
	}

	return 0;
}
