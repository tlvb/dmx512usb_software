#ifndef __DMX512USB_SOFTWARE_SERIAL__
#define __DMX512USB_SOFTWARE_SERIAL__

#include <string>
#include <sstream>

namespace dmx512usb_software {

	class Serial {

		private:
			int sfd;
			bool portopen;
			int baudrate;
			bool ok;
			std::ostringstream log;


		public:
			Serial(const std::string &device, int baudrate);
			~Serial(void);
			bool is_ok(void);
			std::string get_log(void);
			int get_actual_baudrate(void);

			int writebytes(uint8_t *data, unsigned n);
			int readbytes(uint8_t *data, unsigned n);

		private:
			int open_port(const std::string &device);
			void config_port(int fd, int baudrate);
	};


}



#endif
