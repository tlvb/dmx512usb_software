#include "periodicworker.hh"

namespace dmx512usb_software {

	PeriodicWorker::PeriodicWorker(long seconds, long nanoseconds) {
		s = seconds;
		ns = nanoseconds;
		wts = Setup;
	}

	void PeriodicWorker::start(void) {
		run = true;
		wt = std::thread(&PeriodicWorker::loop, this);
	}

	PeriodicWorker::WTState PeriodicWorker::state(void) {
		guard.lock();
		WTState scopy = wts;
		guard.unlock();
		return scopy;
	}

	void PeriodicWorker::abort(void) {
		guard.lock();
		run = false;
		guard.unlock();
	}


	void PeriodicWorker::loop(void) {
#ifdef __linux__
		guard.lock();
		wts = Running;
		guard.unlock();

		init();

		struct timespec t;
		clock_gettime(CLOCK_MONOTONIC, &t);

		bool condition = true;
		while (condition) {
			guard.lock();
			condition = run;
			guard.unlock();
			condition &= work();
			if (t.tv_nsec + ns > 1000000000) {
				t.tv_sec += 1;
				t.tv_nsec -= 1000000000;
			}
			t.tv_sec += s;
			t.tv_nsec += ns;
			clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);
		}

		guard.lock();
		wts = Finished;
		guard.unlock();
#else
#error "you need to implement your own sleep functions"
#endif
	}

	void PeriodicWorker::join(void) {
		if (state() == Running)
			abort();
		wt.join();
	}
}
