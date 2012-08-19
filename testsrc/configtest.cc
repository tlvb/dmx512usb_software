#include "config.hh"
#include <iostream>

using namespace std;
using namespace dmx512usb_sw;

int main(void) {

	Config c;

	if (c.is_ok()) {
		cout << "config ok, device =\"" << c.get_device() << "\"" << endl;
	}
	else {
		cout << "config is not ok, error=\"" << c.get_error() << "\"" << endl;
	}

	return 0;
}

