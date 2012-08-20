#ifndef __DMX512USB_SOFTWARE_CONFIG_HH__
#define __DMX512USB_SOFTWARE_CONFIG_HH__

#include <string>
#include <sstream>
#include <confuse.h>


namespace dmx512usb_software {

	class Config {

		/*
			We are going to interface a C library
			and pass strings back and forth, it is
			bound to be somewhat ugly
		*/
		
		private:
			static const std::string config_file;

		private:
			std::string device;
			std::ostringstream log;
			bool ok;



		
		public:
			Config(void);
			bool is_ok(void);
			std::string get_log(void);

			std::string get_device(void);

		private:
			bool try_dir(std::string dir);


	};
}



#endif
