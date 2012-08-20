#include "config.hh"

#include <cstdlib>
#include <cstring>

namespace dmx512usb_software {

	const std::string Config::config_file = "/dmx512usb_sw/config";

	Config::Config(void) {

		ok = false;
		device = "";

		unsigned ndirs = 2;
		char *dirs[] = {
			std::getenv("XDG_CONFIG_HOME"),
			(char*)"/etc"
		};

		log << "1:" << (void*)dirs[0] << std::endl;
		log << "2:" << (void*)dirs[1] << std::endl;
		
		bool success = false;
		for (unsigned i=0; i<ndirs; ++i) {
			if (dirs[i] != NULL && try_dir(std::string(dirs[i]))) {
				success = true;
				break;
			}
		}
		if (!success)
			log << "no more paths to try, defaults used" << std::endl;
	}

	bool Config::is_ok(void) {
		return ok;
	}

	std::string Config::get_log(void) {
		return log.str();
	}

	std::string Config::get_device(void) {
		return device;
	}

	bool Config::try_dir(std::string dir) {
		std::string path = dir + config_file;
		char *devtmp = strdup((char*)"/dev/ttyUSB0");

		cfg_opt_t opts[] = {
			CFG_SIMPLE_STR((char*)"device", &devtmp),
			CFG_END()
		};

		cfg_t *cfg = cfg_init(opts, 0);

		int ret = cfg_parse(cfg, path.c_str());
		
		device = std::string(devtmp);
		std::free(devtmp);

		switch (ret) {
			case CFG_SUCCESS:
				ok = true;
				cfg_free(cfg);
				log << "using config file \"" << path << "\"" << std::endl;
				return true;
				break;
			case CFG_FILE_ERROR:
				ok = false;
				log << "config file \"" << path << "\" not found" << std::endl;
				return false;
				break;
			case CFG_PARSE_ERROR:
				ok = false;
				log << "parse error in config file \"" << path << "\"" << std::endl;
				return false;
				break;
			default:
				log << "unknown error in config file \"" << path << "\"" << std::endl;
				ok = false;
				return false;
		}
	}
}
