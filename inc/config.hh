#ifndef __DMX512SW_CONFIG_HH__
#define __DMX512SW_CONFIG_HH__

#include <string>
#include <confuse.h>

namespace dmx512usb_sw {

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
			std::string error;
			bool ok;



		
		public:
			Config(void);
			bool is_ok(void);
			std::string get_error(void);

			std::string get_device(void);

		private:
			bool try_dir(std::string dir);


	};
}



#endif
