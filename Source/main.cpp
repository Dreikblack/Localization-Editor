#include "UltraEngine.h"
#include "Application.h"

using namespace UltraEngine;

int main(int argc, const char* argv[]) {

	try {
		auto app = Application::getInstance();
		app->init();
		app->loop();
	} catch (const std::exception& e) {
		auto stream = WriteFile("CrashLog.txt");
		stream->WriteWLine(e.what());
		stream->Close();
		Notify(e.what());
	}

	return 0;
}