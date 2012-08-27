#ifndef __DMX512USB_SOFTWARE_LINK_HH__
#define __DMX512USB_SOFTWARE_LINK_HH__

namespace dmx512usb_software {

	class BaseLink {
		protected:
			MailBox *mailb;
			Pattern *pat;
		public:
			BaseLink(MailBox *mb);
			void set_pattern(Pattern *p);
	}

	class SliderLink {
		protected:
			ActivePattern *apat;
		
		public:
			tgui:EventReaction changecb(int oldv, int newv)

		


}

#endif
