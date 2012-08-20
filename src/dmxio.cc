#include "dmxio.hh"

namespace dmx512usb_software {
	
	uint8_t SourceFold::operator()(const sourcelist &l, unsigned long t) const {
		uint8_t max = 0;
		for (sourcelist::const_iterator i=l.begin(); i!=l.end(); ++i) {
			uint8_t v = i->second(t);
			max = v>max?v:max;
		}
		return max;
	}

	DmxWriter::DmxWriter(std::string device, unsigned nchannels) :
			PeriodicWorker(0, 1000000000/128),
			port(device, 1250000),
			sources(nchannels, std::list<std::pair<int,source>>()),
			stored(nchannels, 0) {

		this->nchannels = nchannels;
		ok = false;
		if (port.is_ok()) {
			ok = true;
			start();
		}
	}

	int DmxWriter::register_source(int channel, source &s) {
		guard.lock();
		int id = idc++;
		sources[channel].push_back(std::pair<int, source>(id, s));
		guard.unlock();
		return id;
	}

	void DmxWriter::unregister_source(unsigned channel, int id) {
		guard.lock();
		sources[channel].remove_if(id_matches(id));
		guard.unlock();
	}

	bool DmxWriter::is_ok(void) {
		return ok;
	}

	void DmxWriter::init(void) {
		uint8_t b = 0xaa;
		for (int i=0; i<256; ++i) {
			port.writebytes(&b, 1);
		}
		b = 0x55;
		port.writebytes(&b, 1);
	}

	bool DmxWriter::work(void) {
		guard.lock();
		unsigned long t = ticc++;
		for (uint8_t c=0; c<nchannels; ++c) {
			uint8_t cv[2] = {c, fold(sources[c], t)};
			if (stored[c] != cv[1]) {
				stored[c] = cv[1];
				port.writebytes(cv, 2);
			}
		}
		guard.unlock();
		return true;
	}



}
