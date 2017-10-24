#include "../Global/stdafx.h"
#include "stdafx.h"
#include "Keys.h"
#include "Event.h"

App::Input::KeySignal::KeySignal(int pKey)
{
	press = EventSignal<KeyEvent>(KeyEvent(pKey, KeyCondition(1, 0))).index();
	release = EventSignal<KeyEvent>(KeyEvent(pKey, KeyCondition(0, 0))).index();
}

void App::Input::reserveKeySignals(size_t pCount)
{
	EventSignal<KeyEvent>::reserve(pCount * 2);
}


void App::Input::key_Callback(GLFWwindow * window, int pKey, int pScancode, int pAction, int pMods)
{
	pushEvent(KeyEvent(pKey, pAction, pMods));
}

void App::Input::char_Callback(GLFWwindow * window, size_t pCodepoint)
{
	//printf("char callBack! Char: %c \n", pCodepoint);
}