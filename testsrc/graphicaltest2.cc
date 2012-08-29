#include <SDL/SDL.h>
#include <functional>
#include <sstream>
#include <iomanip>
#include <mutex>
#include <iostream>

#include "tgui/tgui.hh"

#include "config.hh"
#include "serial.hh"
#include "dmx.hh"
#include "controls.hh"


using namespace dmx512usb_software;



tgui::EventReaction killer(SDL_Event* e) {
	return tgui::QUIT;
}

#define N 16


int main(void) {
	Config config;
	
	std::cout << "config is ok?: " << config.is_ok() << std::endl;
	std::cout << "dumping config log:" << std::endl << "---" << std::endl;
	std::cout << config.get_log() << "---" << std::endl;

	if (!config.is_ok())
		return 1;

	Serial port(config.get_device(), 120000);

	std::cout << "serial is ok?: " << port.is_ok() << std::endl;
	std::cout << "dumping serial log:" << std::endl << "---" << std::endl;
	std::cout << port.get_log() << "---" << std::endl;

	if (!port.is_ok())
		return 1;

	DMX_hw_if dmx(64);

	dmx.set_masterfader(1);
	dmx.set_device(&port);
	dmx.start();

	SDL_Init(SDL_INIT_VIDEO);
	TTF_Init();
	//SDL_Surface *screen = SDL_SetVideoMode(1024, 768, 32, SDL_DOUBLEBUF|SDL_FULLSCREEN);
	SDL_Surface *screen = SDL_SetVideoMode(640, 480, 32, SDL_DOUBLEBUF);

	TTF_Font *font = tgui::load_font("DejaVu Sans Mono", "book", 10);
	tgui::Widget::set_defaults(0xff070f1f, 0xff3f7fff, font);

	tgui::Window w = tgui::Window(screen);

	tgui::VContainer c1 = tgui::VContainer();
	tgui::Grid g(N, 3);

	Control *controls[N];

	for (unsigned i=0; i<N; ++i) {
		controls[i] = new Control(&dmx);
		controls[i]->set_channel(i);
		controls[i]->add_elements_to_grid(&g, i);
		controls[i]->set_eventarbiter(&w);
	}
	controls[N-1]->set_mode(Control::fireforget);


/*
	//tgui::VContainer *chconts[N];

	SDLKey keys[2][N] = {	{SDLK_1,		SDLK_2,		SDLK_3,			SDLK_4, SDLK_5, SDLK_6, SDLK_7,	SDLK_8},
							{SDLK_QUOTE,	SDLK_COMMA, SDLK_PERIOD,	SDLK_p,	SDLK_y, SDLK_f, SDLK_g,	SDLK_c}};
		//w.grab_key(keys[1][i&7], KMOD_NONE, std::bind(&tgui::Widget::handle_event, tbs[i], std::placeholders::_1));
		//chconts[i]->append_child(tbs[i]);
		//g.set_child(tbs[i], i, 3);

		//c2.append_child(chconts[i]);
*/


	tgui::Label l1 = tgui::Label("Quit by pressing both control keys and escape...");
	c1.append_child(&l1);
	//c1.append_child(&c2);
	c1.append_child(&g);

	w.attach_child(&c1);

	w.grab_key(SDLK_ESCAPE, (SDLMod)(KMOD_LCTRL|KMOD_RCTRL), tgui::EventCallback(&killer));
	w.draw();
	SDL_Flip(screen);

    w.event_loop();

	dmx.join();

	for (unsigned i=0; i<N; ++i) {
		delete controls[i];
	}

	TTF_CloseFont(font);
	SDL_Quit();
}

