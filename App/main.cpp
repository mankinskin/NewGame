#include"Global\stdafx.h"
#include "Global\App.h"


int main() {

	App::init();
	while (App::state != App::State::Exit) {
		App::mainMenuLoop();
		App::frameLoop();
	}
	return 0;
}