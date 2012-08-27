#include <SDL/SDL.h>
#include <functional>
#include <sstream>
#include <iomanip>
#include <mutex>
#include <iostream>

#include "tgui/tgui.hh"

#include "config.hh"
#include "dmxio.hh"


using namespace dmx512usb_software;



class ButtonGlue {
	private:
		std::mutex m;
		uint8_t value;
	public:
		ButtonGlue(void) : value(0) {};
		tgui::EventReaction buttoncb(bool press, uint8_t button) {
			{
				std::lock_guard<std::mutex> lock(m);
				if (press)
					value = 0xff;
				else
					value = 0x00;
			}
			std::cout << "buttoncb " << press << std::endl;
			return 0;
		}

		uint8_t get_value(unsigned long time) {
			uint8_t ret;
			{
				ret = value;
			}
			return ret;
		}
};

class SliderGlue : Source {
	private:
		MailBox &mailbox;
		uint8_t value;
	public:
		SliderGlue(MailBox &mb) : mailbox(mb), value(0) {};
		tgui::EventReaction changecb(tgui::Label *l, int oldv, int newv) {
			mailbox.post([&value,=newv]{value = newv});
			l->set_text(ss.str(), false);
			l->draw();
			std::cout << "s changecb " << oldv << "->" << newv << " " << (unsigned)value << std::endl;
			return tgui::UPDATE_SCREEN;
		}

		uint8_t get_value(unsigned long time) {
			uint8_t ret;
			{
				ret = this->value;
			}
			return ret;
		}
};
	

tgui::EventReaction killer(SDL_Event* e) {
	return tgui::QUIT;
}

#define N 32


int main(void) {
	Config config;
	
	std::cout << "config is ok?: " << config.is_ok() << std::endl;
	std::cout << "dumping config log:" << std::endl << "---" << std::endl;
	std::cout << config.get_log() << "---" << std::endl;

	DMXWriter writer(config.get_device(), 32);

	std::cout << "writer is ok?: " << writer.is_ok() << std::endl;
	std::cout << "dumping writer log:" << std::endl << "---" << std::endl;
	std::cout << writer.get_log() << "---" << std::endl;

	SDL_Init(SDL_INIT_VIDEO);
	TTF_Init();
	SDL_Surface *screen = SDL_SetVideoMode(1024, 768, 32, SDL_DOUBLEBUF|SDL_FULLSCREEN);
	//SDL_Surface *screen = SDL_SetVideoMode(640, 480, 32, SDL_DOUBLEBUF);

	TTF_Font *font = tgui::load_font("DejaVu Sans Mono", "book", 10);
	tgui::Widget::set_defaults(0xff070f1f, 0xff3f7fff, font);

	tgui::Window w = tgui::Window(screen);

	tgui::VContainer c1 = tgui::VContainer();
	//tgui::HContainer c2 = tgui::HContainer();
	tgui::Grid g(N, 4);

	//tgui::VContainer *chconts[N];

	tgui::Label *vlbs[N];
	tgui::BaseSlider *vss[N];
	SliderGlue *sglue[N];
	tgui::Button *bs[N];
	ButtonGlue *bglue[N];
	tgui::ToggleButton *tbs[N];

	SDLKey keys[2][N] = {	{SDLK_1,		SDLK_2,		SDLK_3,			SDLK_4, SDLK_5, SDLK_6, SDLK_7,	SDLK_8},
							{SDLK_QUOTE,	SDLK_COMMA, SDLK_PERIOD,	SDLK_p,	SDLK_y, SDLK_f, SDLK_g,	SDLK_c}};

	for (int i=0; i<N; ++i) {
		//chconts[i] = new tgui::VContainer();
		
		vlbs[i] = new tgui::Label("000");
		vlbs[i]->set_gravity(tgui::right, tgui::top);
		//chconts[i]->append_child(vlbs[i]);
		g.set_child(vlbs[i], i, 0);

		vss[i] = new tgui::BaseSlider(true, 256, 0, 255);
		sglue[i] = new SliderGlue();
		vss[i]->set_eventarbiter(&w);
		vss[i]->register_callback(std::bind(&SliderGlue::changecb, sglue[i], vlbs[i], std::placeholders::_1, std::placeholders::_2));
		//chconts[i]->append_child(vss[i]);
		g.set_child(vss[i], i, 1);
		//writer.register_source(i, std::bind(&SliderGlue::get_value, sglue[i], std::placeholders::_1));
		writer.post([=]{writer.register_source(i, std::bind(&SliderGlue::get_value, sglue[i], std::placeholders::_1));});

		bs[i] = new tgui::Button("");
		bglue[i] = new ButtonGlue();
		bs[i]->set_gravity(tgui::center, tgui::top);
		bs[i]->register_callback(std::bind(&ButtonGlue::buttoncb, bglue[i], true, std::placeholders::_1), tgui::mousepress);
		bs[i]->register_callback(std::bind(&ButtonGlue::buttoncb, bglue[i], false, std::placeholders::_1), tgui::mouserelease);
		w.grab_key(keys[0][i&7], KMOD_NONE, std::bind(&tgui::Widget::handle_event, bs[i], std::placeholders::_1));
		//chconts[i]->append_child(bs[i]);
		g.set_child(bs[i], i, 2);
		//writer.register_source(i, std::bind(&ButtonGlue::get_value, bglue[i], std::placeholders::_1));

		tbs[i] = new tgui::ToggleButton("");
		tbs[i]->set_gravity(tgui::center, tgui::top);
		w.grab_key(keys[1][i&7], KMOD_NONE, std::bind(&tgui::Widget::handle_event, tbs[i], std::placeholders::_1));
		//chconts[i]->append_child(tbs[i]);
		g.set_child(tbs[i], i, 3);

		//c2.append_child(chconts[i]);


	}

	tgui::Label l1 = tgui::Label("Quit by pressing both control keys and escape...");
	c1.append_child(&l1);
	//c1.append_child(&c2);
	c1.append_child(&g);

	w.attach_child(&c1);

	w.grab_key(SDLK_ESCAPE, (SDLMod)(KMOD_LCTRL|KMOD_RCTRL), tgui::EventCallback(&killer));
	w.draw();
	SDL_Flip(screen);

    w.event_loop();

	writer.join();

	for (int i=0; i<N; ++i) {
		delete bs[i];
		delete tbs[i];
		delete vss[i];
		delete vlbs[i];
		//delete chconts[i];
	}

	TTF_CloseFont(font);
	SDL_Quit();
}

