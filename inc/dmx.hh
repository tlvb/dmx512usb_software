#ifndef __DMX512USB_SOFTWARE_DMX_HH__
#define __DMX512USB_SOFTWARE_DMX_HH__

#include <vector>
#include <list>
#include <queue>
#include <functional>
#include <mutex>
#include <stdint.h>
#include <string>

#include "periodicworker.hh"
#include "serial.hh"
#include "controls.hh"


namespace dmx512usb_software {

	// {{{ Source class
	class Control;
	class DMX_hw_if;
	class Source {
		protected:
			Control *control;
			DMX_hw_if *dmx;
			bool oneshot;
			uint8_t fadervalue;
		public:
			Source(Control *c, DMX_hw_if *d);
			Control *get_control(void);
			DMX_hw_if *get_dmx(void);

			virtual ~Source(void) {}
			virtual const std::vector<short> &get_channels(void) = 0;
			virtual uint8_t get_value(short channel, unsigned long time) = 0;
			virtual void set_fader(uint8_t value);
			virtual void set_oneshot(bool oneshot);
	};
	// }}}

	// {{{ SimpleSource class
	class SimpleSource : public Source {
		protected:
			std::vector<short> channels;
			bool starttimeset;
			unsigned long starttime;
		public:
			SimpleSource(short channel, Control *c, DMX_hw_if *d);
			virtual const std::vector<short> &get_channels(void);
			virtual uint8_t get_value(short channel, unsigned long time);
	};
	// }}}

	// {{{ Pattern class
	class Pattern {
		public:
			virtual const std::string &get_name(void) = 0;
			virtual Source *create_source(short channel) = 0;
	};
	// }}}
	
	// {{{ MailBox class
	class MailBox {
		protected:
			std::queue<std::function<void(void)>> postbox;
			std::mutex postmutex;
		public:
			void post(std::function<void(void)> letter);
	};
	// }}}

	// {{{ DMX_hw_if class
	class DMX_hw_if : public PeriodicWorker, public MailBox {

		protected:
			unsigned nchannels;
			Serial *port;
			double masterfader;
			std::vector<std::list<Source*>> sources;
			std::vector<uint8_t> stored;
			unsigned long ticc;

		public:
			DMX_hw_if(unsigned nchannels);
			void set_device(Serial *port);
			void register_source(Source *s);
			void unregister_source(Source *s,  bool immediately);
			void set_masterfader(double value);

			void remove_source_soft(Source *s);
			void remove_source_hard(Source *s);

		protected:
			virtual bool work(void);


	};
	// }}}



}


#endif
