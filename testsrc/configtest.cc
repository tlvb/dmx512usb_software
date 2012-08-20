#include "config.hh"
#include <iostream>

using namespace std;
using namespace dmx512usb_software;

int main(void) {

	Config c;

	if (c.get_device().compare("/dev/ttyUSB0") != 0) {
		cout << "c.get_device() does does not equal expected value" << endl;
		cout << "expected: \"/dev/ttyUSB0\"" << endl;
		cout << "actual:   \"" << c.get_device() << "\"" << endl;
		return 1;
	}
	
	cout << "configtest success" << endl;
	return 0;
}

