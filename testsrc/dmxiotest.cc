#include "dmxio.hh"
#include "config.hh"

#include <functional>
#include <mutex>
#include <iostream>

using namespace dmx512usb_software;


int main(void) {

	std::mutex m;
	uint8_t value = 0;

	Config config;

	DmxWriter writer(config.get_device(), 32);

	source s = source([&m, &value](unsigned long t) ->
		uint8_t {
			m.lock();
			uint8_t ret = value;
			m.unlock();
			return ret;
		});
	writer.register_source(0,s);

	
	bool work = true;
	while (work) {
		int input;
		std::cout << "value >";
		std::cin >> input;
		if (input < 0 || input > 255) {
			work = false;
		}
		else {
			m.lock();
			value = (uint8_t)input;
			m.unlock();
		}
	}

	writer.join();	

}
