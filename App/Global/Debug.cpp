#include"stdafx.h"
#include "Debug.h"
#include "App.h"
#include <conio.h>
#include <cstdio>
#include <algorithm>

int App::Debug::shouldPrintInfo;
std::vector<App::Debug::Error> App::Debug::errorBuffer;

void App::Debug::pushError(std::string pMessage, Error::Severity errorSeverity)
{
	errorBuffer.push_back(Error(pMessage, errorSeverity));
}

void App::Debug::printErrors()
{
	int fatal = 0;
	unsigned int errorCount = errorBuffer.size();
#ifdef _DEBUG
	if (errorCount) {
		printf("\n%i Error(s) or Warning(s) occured.\n", errorCount);
	}
#endif
	for (Error& err : errorBuffer) {
		std::string severityString;
		switch (err.severity) {
		case Error::Severity::Warning:
			severityString = "Warning";
			break;

		case Error::Severity::Trivial:
			severityString = "Trivial Error";
			break;

		case Error::Severity::Fatal:
			fatal = 1;
			severityString = "Fatal Error";
			break;
		}
#ifdef _DEBUG
		printf("%s: %s\n", severityString.c_str(), err.msg.c_str());
#endif
	}
	errorBuffer.clear();
	if (fatal) {
		//break program
		App::state = App::State::Exit;
		puts("Press any key to quit...");
		while (!_getch()) {};
	}
	
}

void App::Debug::togglePrintInfo() {
	shouldPrintInfo = !shouldPrintInfo;
}

void App::Debug::printInfo()
{
	if (shouldPrintInfo) {
		App::lastFrameMS = std::max(1.0, App::lastFrameMS);
		//system("CLS");
		printf("\nlastFrameMS %i\n", (int)App::lastFrameMS);
		printf("FPS\nActual %i\nPotential %i\n", (unsigned int)(1000 / (App::lastFrameMS + std::max((int)0, (int)(App::targetFrameMS - App::lastFrameMS)))), (unsigned int)(1000 / (App::lastFrameMS)));

	}
}
