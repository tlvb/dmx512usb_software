#include "periodicworker.hh"

PeriodicWorker::PeriodicWorker(long seconds, long nanoseconds) {
	s = seconds;
	ns = nanoseconds;
	wts = Setup;
}

void PeriodicWorker::start(void) {
	wt = boost::thread(&PeriodicWorker::loop, this);
}

PeriodicWorker::WTState PeriodicWorker::state(void) {
	return wts;
}


void PeriodicWorker::loop(void) {
#ifdef __linux__
	wts = Running;

	struct timespec t;
	clock_gettime(CLOCK_MONOTONIC, &t);

	while(work()) {
		if (t.tv_nsec + ns > 1000000000) {
			t.tv_sec += 1;
			t.tv_nsec -= 1000000000;
		}
		t.tv_sec += s;
		t.tv_nsec += ns;
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);
	}

	wts = Finished;

#else
#error "you need to implement your own sleep functions"
#endif
}

void PeriodicWorker::join(void) {
	wt.join();
}
