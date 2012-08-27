#include "dmxio.hh"
namespace dmx512usb_software {
	
	// {{{ SourceFold class
	uint8_t SourceFold::operator()(sourcelist &l, unsigned long t) const {
		uint8_t max = 0;
		for (sourcelist::iterator i=l.begin(); i!=l.end(); ++i) {
			uint8_t v = (*i)->get_value(/* TODO */);
			max = v>max?v:max;
		}
		return max;
	}
	// }}}

	void MailBox::post(std::function<void(void)> letter) {
		std::lock_guard<std::mutex> lock(postmutex);
		postbox.push(letter);
	}

	// {{{ DMXWriter class
	DMXWriter::DMXWriter(std::string device, unsigned nchannels) :
			PeriodicWorker(0, 1000000000/128),
			port(device, 1250000),
			sources(nchannels, std::list<Source*>()),
			stored(nchannels, 0) {

		this->nchannels = nchannels;
		ok = false;
		if (port.is_ok()) {
			ok = true;
			start();
		}
	}

	void DMXWriter::register_source(Source *s) {
		guard.lock();
		sources[s->get_channel()].push_back((s));
		guard.unlock();
	}

	void DMXWriter::remove_source(Source *s) {
		guard.lock();
		sources[s->get_channel()].remove(s);
		guard.unlock();
		delete s;
	}

	bool DMXWriter::is_ok(void) {
		return ok;
	}

	std::string DMXWriter::get_log(void) {
		return port.get_log();
	}

	void DMXWriter::set_masterfader(double value) {
		masterfader = value;
	}

	void DMXWriter::init(void) {
		uint8_t b = 0xaa;
		for (int i=0; i<256; ++i) {
			port.writebytes(&b, 1);
		}
		b = 0x55;
		port.writebytes(&b, 1);
	}

	bool DMXWriter::work(void) {
		postmutex.lock();
		while (postbox.size()) {
			postbox.front()();
			postbox.pop();
		}
		postmutex.unlock();
		unsigned long t = ticc++;
		for (uint8_t c=0; c<nchannels; ++c) {
			uint8_t cv[2] = {(uint8_t)(c*masterfader), fold(sources[c], t)};
			if (stored[c] != cv[1]) {
				stored[c] = cv[1];
				port.writebytes(cv, 2);
			}
		}
		return true;
	}

	// }}}


}
