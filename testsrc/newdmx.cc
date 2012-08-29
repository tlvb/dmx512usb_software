#include "dmx.hh"

using namespace dmx512usb_software;

int main(void) {
	DMX_hw_if hwif(64);
	hwif.start();
	Serial port("/dev/ttyUSB0", 1250000);
	hwif.post([&]{hwif.set_device(&port);});

	SimpleSource ss(0);
	hwif.post([&]{hwif.register_source(&ss);});
	hwif.post([&]{hwif.set_masterfader(1);});
	hwif.post([&]{ss.set_value(0xff);});
	system("sleep 0.5");
	hwif.post([&]{hwif.set_masterfader(0.5);});
	system("sleep 0.5");
	hwif.post([&]{ss.set_value(0xff);});
	hwif.post([&]{hwif.set_masterfader(1.0/255.0);});


	system("sleep 5s");
	hwif.join();

	return 0;
}
