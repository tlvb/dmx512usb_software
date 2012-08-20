#include "config.hh"
#include "serial.hh"

#include <iostream>

using namespace dmx512usb_software;

int main(void) {

	Config conf;

	std::cout << "-----" << std::endl;
	std::cout << conf.get_log() << std::endl;
	std::cout << "-----" << std::endl;

	Serial serial(conf.get_device(), 1250000);
	
	std::cout << "-----" << std::endl;
	std::cout << serial.get_log() << std::endl;
	std::cout << "-----" << std::endl;

}

