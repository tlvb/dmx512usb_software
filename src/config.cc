#include "config.hh"
#include <cstdlib>
#include <cstring>


namespace dmx512usb_sw {

	const std::string Config::config_file = "/dmx512usb_sw/config";

	Config::Config(void) {

		error = "";
		ok = false;
		device = "";

		int ndirs = 2;
		std::string dirs[] = {
			std::string(std::getenv("XDG_CONFIG_HOME")),
			"/etc"
		};

		for (int i=0; i<ndirs; ++i) {
			if (try_dir(dirs[i]))
				break;
		}
	}

	bool Config::is_ok(void) {
		return ok;
	}

	std::string Config::get_error(void) {
		return error;
	}

	std::string Config::get_device(void) {
		return device;
	}

	bool Config::try_dir(std::string dir) {
		std::string path = dir + config_file;
		char *devtmp = strdup((char*)"/dev/ttyUSB0");

		cfg_opt_t opts[] = {
			CFG_SIMPLE_STR((char*)"device", &devtmp)
		};

		cfg_t *cfg = cfg_init(opts, 0);
		int ret = cfg_parse(cfg, path.c_str());
		cfg_free(cfg);

		switch (ret) {
			case CFG_SUCCESS:
				ok = true;
				error = "no error";
				device = std::string(devtmp);
				std::free(devtmp);
				return true;
				break;
			case CFG_FILE_ERROR:
				ok = false;
				error = "config file not found";
				return false;
				break;
			case CFG_PARSE_ERROR:
				ok = false;
				error = "error in config file";
				return false;
				break;
			default:
				error = "unknown error";
				ok = false;
				return false;
		}
	}
}
