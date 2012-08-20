#include <iostream>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <linux/serial.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "serial.hh"

namespace dmx512usb_software {

	Serial::Serial(std::string device, int baudrate) {
		portopen = false;
		sfd = open_port(device.c_str());
		if (ok)
			config_port(sfd, baudrate);
	}

	Serial::~Serial(void) {
		if (portopen)
			close(sfd);
	}

	bool Serial::is_ok(void) {
		return ok;
	}

	std::string Serial::get_log(void) {
		return log.str();
	}

	int Serial::get_actual_baudrate(void) {
		return baudrate;
	}



	int Serial::writechars(char *data, int n) {
		return write(sfd, data, n);
	}

	int Serial::readchars(char *data, int n) {
		return read(sfd, data, n);
	}




	int Serial::open_port(const char *device) {
		int sfd = open(device, O_RDWR | O_NOCTTY | O_NDELAY);
		if (sfd == -1) {
			ok = false;
			log << "unable to open port \"" << device << "\"" << std::endl;
		}
		else {
			fcntl(sfd, F_SETFL, 0);
			ok = true;
			log << "port \"" << device << "\" open" << std::endl;
			portopen = true;
		}
		return sfd;
	}

	void Serial::config_port(int sfd, int baudrate) {
		struct termios tos;
		struct serial_struct ss;
		int ret;

		ioctl(sfd, TIOCGSERIAL, &ss);
		ss.flags = (ss.flags & ~ASYNC_SPD_MASK) | ASYNC_SPD_CUST;
		ss.custom_divisor = (ss.baud_base + (baudrate / 2)) / baudrate;
		this->baudrate = ss.baud_base / ss.custom_divisor;

		if (this->baudrate < baudrate * 98 / 100 || this->baudrate > baudrate * 102 / 100) {
			ok = false;
			log << "Cannot set serial port baudrate to " << baudrate << " Closest possible is " << this->baudrate << std::endl;
		}

		ret = ioctl(sfd, TIOCSSERIAL, &ss);

		cfsetispeed(&tos, B38400);
		cfsetospeed(&tos, B38400);
		
		tos.c_cflag &= ~(PARENB | CSTOPB | CSIZE);
		tos.c_cflag |= CS8;
		tos.c_cflag |= CRTSCTS;
		tos.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
		tos.c_iflag &= ~(INPCK | IGNPAR | PARMRK | ISTRIP | IXON | IXOFF);
		tos.c_iflag &= ~(IXANY | BRKINT | INLCR | IGNCR | ICRNL | IUCLC | IMAXBEL);
		tos.c_oflag &= ~OPOST;
		ret = tcsetattr(sfd, TCSANOW, &tos);
		if (ret != 0) {
			ok = false;
			log << "tcsetattr returned nonzero" << std::endl;
		}

	}



}
/*
int main(void) {
	int sfd = open_port("/dev/ttyUSB0");
	config_port(sfd);
	char c[16];
	c[15] = 0;
	for (;;) {
		size_t count = read(sfd, c, 15);
		if (count > 0) {
			std::cout << "read " << count << "bytes: '";
			for (size_t i=0; i<count; ++i) {
				std::cout << "'" << c[i];
			}
			std::cout << "'" << std::endl;
		}
	}
}
*/
