#include "dmx.hh"

#include <iostream>

namespace dmx512usb_software {
	
	// {{{ Source class
	Source::Source(Control *c, DMX_hw_if *d) :
			control(c),
			dmx(d),
			oneshot(false),
			fadervalue(255) {}

	Control *Source::get_control(void) {
		return control;
	}

	DMX_hw_if *Source::get_dmx(void) {
		return dmx;
	}

	void Source::set_fader(uint8_t value) {
		fadervalue = value;
	}

	void Source::set_oneshot(bool oneshot) {
		this->oneshot = oneshot;
	}
	// }}}

	// {{{ SimpleSource class
	SimpleSource::SimpleSource(short channel, Control *c, DMX_hw_if *d) :
		Source(c, d),
		channels(1,channel) {}

	const std::vector<short> &SimpleSource::get_channels(void) {
		return channels;
	}

	uint8_t SimpleSource::get_value(short channel, unsigned long time) {
		if (oneshot) {
			if (!starttimeset) {
				starttime = time;
				starttimeset = true;
			}
			else {
				if (time > starttime + 128) {
					std::cout<<"time has run out"<<std::endl;
					dmx->post([=]{dmx->remove_source_soft(this);});
					return 0;
				}
			}
		}
		return fadervalue;
	}
	// }}}


	// {{{ MailBox class
	void MailBox::post(std::function<void(void)> letter) {
		std::lock_guard<std::mutex> lock(postmutex);
		postbox.push(letter);
	}
	// }}}

	// {{{ DMX_hw_if class
	// {{{ public methods
	DMX_hw_if::DMX_hw_if(unsigned nchannels) :
			PeriodicWorker(0, 1000000000/128),
			nchannels(nchannels),
			port(NULL),
			masterfader(0),
			sources(nchannels, std::list<Source*>()),
			stored(nchannels, 0),
			ticc(0) {
		this->nchannels = nchannels;
	}

	void DMX_hw_if::set_device(Serial *port) {
		this->port = port;
		uint8_t b = 0xaa;
		for (int i=0; i<256; ++i) {
			port->writebytes(&b, 1);
		}
		b = 0x55;
		port->writebytes(&b, 1);

	}

	void DMX_hw_if::register_source(Source *s) {
		for (const short &channel : s->get_channels()) {
			sources[channel].push_back(s);	
		}
	}

	void DMX_hw_if::unregister_source(Source *s, bool immediately) {
		if (immediately) {
			for (const short &channel : s->get_channels()) {
				sources[channel].remove(s);	
			}
		}
		else {
			s->set_oneshot(true);
		}
	}

	void DMX_hw_if::set_masterfader(double value) {
		masterfader = value;
	}

	void DMX_hw_if::remove_source_soft(Source *s) {
		std::cout << "[[DMX REMOVE SOFT]]"<<std::endl;
		for (const short &channel : s->get_channels()) {
			sources[channel].remove(s);
		}
		SDL_Event e;
		e.type = SDL_USEREVENT;
		e.user.code = tgui::void_void_callback;
		e.user.data1 = new std::function<void(void)>([=]{s->get_control()->remove_source_hard(s);});
		SDL_PushEvent(&e);
	}

	void DMX_hw_if::remove_source_hard(Source *s) {
		std::cout << "[[DMX REMOVE HARD]]"<<std::endl;
		delete s;
	}

	// }}}

	// {{{ protected methods
	bool DMX_hw_if::work(void) {
		postmutex.lock();
		while (postbox.size()) {
			postbox.front()();
			postbox.pop();
		}
		postmutex.unlock();

		if (port == NULL) {
			return true;
		}

		unsigned long t = ticc++;

		for (uint8_t c=0; c<nchannels; ++c) {
			uint8_t cv[2] = {c, 0};
			for (Source *&s : sources[c]) {
				uint8_t prospect = s->get_value(c, t);
				cv[1] = prospect > cv[1] ? prospect : cv[1];
			}
			cv[1] *= masterfader;
			if (stored[c] != cv[1]) {
				std::cout << "ch " << (unsigned)c << " " << (unsigned) stored[c] << "->" << (unsigned) cv[1] << std::endl;
				stored[c] = cv[1];
				port->writebytes(cv, 2);
			}
		}
		return true;
	}

	// }}}
	// }}}


}
