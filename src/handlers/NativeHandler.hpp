#ifndef handlers_NativeHandler_hpp_included
#define handlers_NativeHandler_hpp_included

namespace native
{
	void install();

	#if defined(_WIN32)

	#include <windows.h>

	bool handle_signal(unsigned int signal) 
	{
		switch (signal) 
		{
			case CTRL_C_EVENT:
				exit(EXIT_SUCCESS);
			case CTRL_CLOSE_EVENT:
				return true;
			default:
				return false;
		}
	}

	// Windows installer
	void install() 
	{
		SetConsoleCtrlHandler((PHANDLER_ROUTINE)handle_signal, true);
	}

	#elif defined(__linux__) || defined(__APPLE__)

	#include <csignal>
	#include <cstdlib>

	void handle_signal(int signal) 
	{
		if (signal != SIGINT)
			return;

		std::exit(EXIT_SUCCESS);
	}

	// *nix and Mac installer
	void install() 
	{
		struct sigaction sig_int_handler;
		sig_int_handler.sa_handler = handle_signal;
		sigemptyset(&sig_int_handler.sa_mask);
		sig_int_handler.sa_flags = 0;

		sigaction(SIGINT, &sig_int_handler, nullptr);
	}

	#endif
	
}

#endif
