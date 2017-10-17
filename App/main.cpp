#include"Global\stdafx.h"
#include "Global\App.h"
#include <conio.h>
int main() {



	App::init();
	while (App::state != App::State::Exit) {
		App::mainMenuLoop();
	}
	//puts("Push any key to quit...");
	//_getch();
	return 0;
}