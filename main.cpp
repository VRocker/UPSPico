#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include "gpiohandler.h"

#define PULSE_PIN 22
#define SHUTDOWN_PIN 27

#define SHUTDOWN_SCRIPT "/opt/upspico/shutdown.sh"

static bool g_isRunning = true;

void sig_handler(int signo)
{
	// A SIGTERM or SIGINT has been fired, tell the application to stop running
	g_isRunning = false;
}

void daemonise( void )
{
	int pid = 0;

	if ( (pid = fork()) < 0 )
	{
		// It forking failed!
		exit( 1 );
	}

	if ( pid > 0 )
		exit( 0 );
	else
	{
		// Daemonising
		setsid();
	}
}

int main()
{
	// Throw the application in the background
	daemonise();

	// Handle signals
	signal(SIGTERM, sig_handler);
	signal(SIGINT, sig_handler);

	// Setup the pulser
	gpiohandler::getSingleton()->SetDirection(PULSE_PIN, GPIO_Direction::Out);
	// Setup the thing that shouts to shutdown
	gpiohandler::getSingleton()->SetDirection(SHUTDOWN_PIN, GPIO_Direction::In);

	while (g_isRunning)
	{
		gpiohandler::getSingleton()->WriteGPIO(PULSE_PIN, true);
		usleep(250000);
		gpiohandler::getSingleton()->WriteGPIO(PULSE_PIN, false);

		// Pin gets pulled low when a shutdown is to be executed
		if (!gpiohandler::getSingleton()->ReadGPIO(SHUTDOWN_PIN))
		{
			// UPS is telling us to shutdown, execute our custom script
			system(SHUTDOWN_SCRIPT);

			// Shutdown script has been executed, stop running
			g_isRunning = false;
		}

		usleep(250000);
	}

	// Cleanup GPIO memory mapping
	gpiohandler::RemoveSingleton();
}
