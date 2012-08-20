#ifndef __PERIODIC_WORKER_HH__
#define __PERIODIC_WORKER_HH__

#include <boost/thread.hpp>
#include <time.h>

class PeriodicWorker {

	public:
		enum WTState {
			Setup,
			Running,
			Finished
		};
	private:
		boost::thread wt;
		WTState wts;
		long s, ns;

	public:
		PeriodicWorker(long seconds, long nanoseconds);
		void start(void);
		WTState state(void);
		void join(void);

	protected:
		virtual bool work(void) = 0;
	
	private:
		void loop(void);
	
	

};

#endif
