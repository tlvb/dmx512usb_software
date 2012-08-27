#ifndef __DMX512USB_SOFTWARE_DMX_IO_HH__
#define __DMX512USB_SOFTWARE_DMX_IO_HH__

#include <functional>
#include <stdint.h>
#include <list>
#include <vector>
#include <queue>
#include <utility>
#include "periodicworker.hh"
#include "serial.hh"
#include "pattern.hh"

namespace dmx512usb_software {
	
	typedef std::function<uint8_t(unsigned long)> source;
	typedef std::list<Source*> sourcelist;

	class SourceFold {
		public:
			virtual uint8_t operator()(sourcelist &l, unsigned long t) const;
	};

	class MailBox {
		protected:
			std::queue<std::function<void(void)>> postbox;
			std::mutex postmutex;
		public:
			void post(std::function<void(void)> letter);
	};

	class DMXWriter : public PeriodicWorker, public Sink, public MailBox {
		private:
			unsigned nchannels;
			std::mutex guard;
			int idc;
			unsigned long ticc;
			Serial port;
			std::vector<sourcelist> sources;
			std::vector<uint8_t> stored;
			SourceFold fold;
			bool ok;
			double masterfader;

		public:
			DMXWriter(std::string device, unsigned nchannels);
			virtual void register_source(Source *s);
			virtual void remove_source(Source *s);
			bool is_ok(void);
			std::string get_log(void);

			void set_masterfader(double value);

		protected:
			virtual bool work(void);
			virtual void init(void);

		private:
			class id_matches {
				int id;
				public:
					id_matches(int id) : id(id) {};
					bool operator()(const std::pair<int, source> &p) {
						return p.first == id;
					}
			};
			
	};

}

#endif
