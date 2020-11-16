/*
 * \brief  Integration test for interplay of the following
 *         features:
 *           - libc
 *           - pthread
 *           - vfs_pipe
 * \author Pirmin Duss
 * \date   2020-11-11
 */

/*
 * Copyright (C) 2020 Genode Labs GmbH
 * Copyright (C) 2020 gapfruit AG
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU Affero General Public License version 3.
 */


/* libc includes */
#include <string.h>
#include <errno.h>
#include <sys/select.h>

/* stdcxx includes */
#include <vector>

/* local includes */
#include "fd_set.h"
#include "input_sender.h"
#include "stdcxx_log.h"
#include "thread.h"


namespace Integration_test
{
	using namespace std;

	class Main;
}


/*
 * Thread-function that runs the test.
 */
void *test_runner(void *)
{
	using namespace Integration_test;
	using namespace std;

	enum { OUTPUT_REDUCTION_FACTOR = 100 };

	Input_sender sender           { };
	Thread_list  threads          { };
	size_t       threads_started  { 0 };

	for (int thread=0; thread<PARALLEL_WORKERS; ++thread) {
		auto handle { threads.add_worker() };
		sender.add_worker(handle.first, handle.second);
		++threads_started;
	}

	while (threads_started < NUMBER_OF_WORKERS) {

		static size_t cnt { 0 };
		if ((cnt++ % OUTPUT_REDUCTION_FACTOR) == 0) {
			log((threads_started - threads.count()), "  workers finished, ",
			    threads.count(), " currently running");
		}

		auto fds { threads.fds() };
		auto num_ready = select(threads.max_fd()+1, &fds, nullptr, nullptr, nullptr);

		if (num_ready == -1) {
			error("select() failed with '", strerror(errno), "'");
			exit(-1);
		}

		vector<size_t> finished         { };
		for (auto fd : threads.descriptor_set()) {

			if (!FD_ISSET(fd, &fds))
					continue;

			auto it = threads.find_worker_by_fd(fd);

			if (it == threads.end()) {
				error("worker not found");
				exit(-2);
			}

			uint8_t buf[16*1024] { };
			auto res { read(fd, buf, sizeof(buf)) };
			if (res < 0) {
				error("read error: fd=", fd);
			} else {
				it->append_result_data(buf, res);
			}

			if (it->done()) {
				finished.push_back(it->worker_no());
			}
		}

		sender.remove_finished_workers(finished);
		threads.remove_finished_workers(finished);

		/* restart more threads when some threads are finished */
		while (threads.count() < PARALLEL_WORKERS && threads_started < NUMBER_OF_WORKERS) {
			auto handle { threads.add_worker() };
			sender.add_worker(handle.first, handle.second);
			++threads_started;
		}
	}

	log("--- test finished ---");
	exit(0);

	return 0;
}


int main(int /*argc*/, char ** /*argv[]*/)
{
	pthread_t thr;

	pthread_create(&thr, nullptr, test_runner, nullptr);

	pthread_join(thr, nullptr);
}
