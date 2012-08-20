#ifndef __DMX512USB_SOFTWARE_DMX_IO_HH__
#define __DMX512USB_SOFTWARE_DMX_IO_HH__

#include <functional>
#include <stdint.h>
#include <list>
#include <vector>
#include <utility>
#include "periodicworker.hh"
#include "serial.hh"

namespace dmx512usb_software {
	
	typedef std::function<uint8_t(unsigned long)> source;
	typedef std::list<std::pair<int, source>> sourcelist;

	class SourceFold {
		public:
			virtual uint8_t operator()(const sourcelist &l, unsigned long t) const;
	};

	class DmxWriter : public PeriodicWorker {
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

		public:
			DmxWriter(std::string device, unsigned nchannels);
			int register_source(int channel, source &s);
			void unregister_source(unsigned channel, int id);
			bool is_ok(void);

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
