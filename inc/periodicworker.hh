#ifndef __DMX512USB_SOFTWARE_PERIODIC_WORKER_HH__
#define __DMX512USB_SOFTWARE_PERIODIC_WORKER_HH__

#include <time.h>
#include <thread>
#include <mutex>

namespace dmx512usb_software {
	class PeriodicWorker {

		public:
			enum WTState {
				Setup,
				Running,
				Finished
			};
		private:
			std::thread wt;
			std::mutex guard;
			WTState wts;
			long s, ns;
			bool run;

		public:
			PeriodicWorker(long seconds, long nanoseconds);
			void start(void);
			WTState state(void);
			void abort(void);
			void join(void);

		protected:
			virtual void init(void) {};
			virtual bool work(void) = 0;
		
		private:
			void loop(void);

	};
}

#endif
