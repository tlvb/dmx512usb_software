#include "patterns.hh"

namespace dmx512usb_software {

	BaseLink(MailBox *mb) : mailb(mb) {}

	BaseLink::set_pattern(Pattern *p) {
		pat = p;
	}

	tgui:EventReaction SliderLink::changecb(int oldv, int newv) {
		if (newv == 0) {

		}
		else {
			if (apat == NULL) {
				apat = new ActivePattern
			}
			else {
				mb.post([=newv]{apat->set_localfader(newv/255);});
		}



}
