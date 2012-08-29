#ifndef __DMX512USB_SOFTWARE_CONTROLS_HH__
#define __DMX512USB_SOFTWARE_CONTROLS_HH__

#include "dmx.hh"
#include "tgui/tgui.hh"

namespace dmx512usb_software {

	class DMX_hw_if;
	class Pattern;
	class Source;
	class Control {
		public:
			enum Mode {
				staticlight,
				fireforget
			};

		protected:
			DMX_hw_if *dmx;
			short channel;
			Mode mode;
			bool flashv;
			uint8_t faderv;
			bool killonzero;
			tgui::VerticalSlider fader;
			tgui::Button flashb;
			tgui::Button killb;
			Pattern *p;
			std::list<Source*> sources;

		public:
			Control(DMX_hw_if *hwif);
			void add_elements_to_grid(tgui::Grid *g, unsigned column);
			void set_eventarbiter(tgui::EventArbiter *ea);

			void set_channel(short c);
			void set_mode(Mode m);
			void set_killonzero(bool kill);

			tgui::EventReaction fadercb(int oldv, int newv);
			tgui::EventReaction flashcb(tgui::MouseAction ma, uint8_t button);
			tgui::EventReaction killcb(tgui::MouseAction ma, uint8_t button);

			void remove_source_soft(Source *s);
			void remove_source_hard(Source *s);

		protected:
			void add_source(void);
			void update_source_faders(void);
			void remove_sources();
	};



}



#endif
