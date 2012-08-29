#include "controls.hh"

#define CONTROLS_DEBUG

#ifdef CONTROLS_DEBUG
#include <iostream>
#include <string>
#define d(x) {std::cout << x << std::endl;}
#else
#define d(x) {}
#endif

namespace dmx512usb_software {

	Control::Control(DMX_hw_if *hwif) :
			dmx(hwif),
			channel(0),
			mode(staticlight),
			flashv(false),
			faderv(0),
			killonzero(true),
			fader(256, 0, 255, 1),
			flashb(""),
			killb(""),
			p(NULL) {

		fader.set_gravity(tgui::center, tgui::top);
		flashb.set_gravity(tgui::center, tgui::top);
		killb.set_gravity(tgui::center, tgui::top);

		fader.register_callback([&](int o, int n) {return fadercb(o, n);});
		flashb.register_callback([&](uint8_t b) {return flashcb(tgui::mousepress, b);}, tgui::mousepress);
		flashb.register_callback([&](uint8_t b) {return flashcb(tgui::mouserelease, b);}, tgui::mouserelease);
		flashb.register_callback([&](uint8_t b) {return flashcb(tgui::mouseclick, b);}, tgui::mouseclick);
		killb.register_callback([&](uint8_t b) {return killcb(tgui::mousepress, b);}, tgui::mousepress);
		killb.register_callback([&](uint8_t b) {return killcb(tgui::mouserelease, b);}, tgui::mouserelease);
		killb.register_callback([&](uint8_t b) {return killcb(tgui::mouseclick, b);}, tgui::mouseclick);

	}

	void Control::add_elements_to_grid(tgui::Grid *g, unsigned column) {
		unsigned row = 0;
		g->set_child(&fader, column, row++);
		g->set_child(&flashb, column, row++);
		g->set_child(&killb, column, row++);
	}

	void Control::set_eventarbiter(tgui::EventArbiter *ea) {
		fader.set_eventarbiter(ea);
	}

	void Control::set_channel(short c) {
		channel = c;
	}

	void Control::set_mode(Mode m) {
		mode = m;
	}

	void Control::set_killonzero(bool kill) {
		killonzero = kill;		
	}

	tgui::EventReaction Control::fadercb(int oldv, int newv) {
		d("fadercb");
		faderv = newv;
		d("faderv = "<<(unsigned)faderv);
		d("fa sources: "<<sources.size());
		if (mode == staticlight) {
			if (sources.empty() && faderv > 0)
				add_source();
			else
				update_source_faders();
		}
		else {
			update_source_faders();
		}
		d("fa sources: "<<sources.size());
		return 0;
	}

	tgui::EventReaction Control::flashcb(tgui::MouseAction ma, uint8_t button) {
		d("flashcb");
		d("fl sources: "<<sources.size());
		switch (ma) {
			case tgui::mousepress:
				d("fl mousepress");
				flashv = true;
				if (mode == staticlight) {
					if (sources.empty()) {
						add_source();
					}
					else {
						update_source_faders();
					}
				}
				else {
					add_source();
				}
				break;
			case tgui::mouserelease:
				d("fl mouserelease");
				flashv = false;
				if (mode == staticlight && !sources.empty()) {
					update_source_faders();
				}
				break;
		}
		d("fl sources: "<<sources.size());
		return 0;
	}
	
	tgui::EventReaction Control::killcb(tgui::MouseAction ma, uint8_t button) {
		d("killcb");
		return 0;
	}

	void Control::remove_source_soft(Source *s) {
		d("[[CONTROL REMOVE SOFT]]");
		sources.remove(s);
		dmx->post([=]{dmx->remove_source_soft(s);});
	}

	void Control::remove_source_hard(Source *s) {
		d("[[CONTROL REMOVE HARD]]");
		sources.remove(s);
		dmx->post([=]{dmx->remove_source_hard(s);});
	}

	void Control::add_source(void) {
		d("add_source");
		uint8_t fader = flashv ? 0xff : faderv;
		Source *s;
		if (p == NULL)
			s = new SimpleSource(channel, this, dmx);
		else 
			s = p->create_source(channel);
		s->set_fader(fader);
		if (mode == fireforget) {
			s->set_oneshot(true);
		}
		dmx->post([=]{dmx->register_source(s);std::cout<<"hello"<<std::endl;});
		sources.push_back(s);
	}

	void Control::update_source_faders(void) {
		d("update_source_faders");
		uint8_t fader = (flashv && mode==staticlight) ? 0xff : faderv;
		d("flashv = " << flashv << " faderv = " << (unsigned) faderv);
		if (fader > 0 || !killonzero) {
			for (Source *&s : sources) {
				dmx->post([=]{s->set_fader(fader);});
			}
		}
		else {
			remove_sources();
		}
		d("sources: "<<sources.size());
	}

	void Control::remove_sources(void) {
		d("remove_sources");
		while (sources.size() > 0) {
			Source *s = sources.front();
			sources.pop_front();
			dmx->post([=]{dmx->remove_source_soft(s);});
		}
	}
}
