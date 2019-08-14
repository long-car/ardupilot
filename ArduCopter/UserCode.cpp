#include "Copter.h"
#include "diversion.h"

#ifdef USERHOOK_INIT
void Copter::userhook_init()
{
    // put your initialisation code here
    // this will be called once at start-up
}
#endif

#ifdef USERHOOK_FASTLOOP
void Copter::userhook_FastLoop()
{
    // put your 100Hz code here
}
#endif

#ifdef USERHOOK_50HZLOOP
void Copter::userhook_50Hz()
{
    // put your 50Hz code here
}
#endif

#ifdef USERHOOK_MEDIUMLOOP
void Copter::userhook_MediumLoop()
{
    // put your 10Hz code here
}
#endif

#ifdef USERHOOK_SLOWLOOP
void Copter::userhook_SlowLoop()
{
    // put your 3.3Hz code here
}
#endif

#ifdef USERHOOK_SUPERSLOWLOOP
void Copter::userhook_SuperSlowLoop()
{
    // put your 1Hz code here
    userTimer++;
    if (userTimer == 150) {
		cmd.id = MAV_CMD_NAV_WAYPOINT;
		cmd.p1 = 0;
		cmd.content.location = Location {
			296719348,
			-986691232,
			100,
			Location::AltFrame::ABOVE_HOME
		};
		cmd_lst[0] = cmd;

		cmd.content.location = Location {
			296720575,
			-986692617,
			100,
			Location::AltFrame::ABOVE_HOME
		};
		cmd_lst[1] = cmd;

		cmd.content.location = Location {
			296719987,
			-986694616,
			100,
			Location::AltFrame::ABOVE_HOME
		};
		cmd_lst[2] = cmd;

    	if (!Diversion().divert(cmd_lst, 3)) hal.console->printf("Failed to Divert\n");
    }
}
#endif

#ifdef USERHOOK_AUXSWITCH
void Copter::userhook_auxSwitch1(uint8_t ch_flag)
{
    // put your aux switch #1 handler here (CHx_OPT = 47)
}

void Copter::userhook_auxSwitch2(uint8_t ch_flag)
{
    // put your aux switch #2 handler here (CHx_OPT = 48)
}

void Copter::userhook_auxSwitch3(uint8_t ch_flag)
{
    // put your aux switch #3 handler here (CHx_OPT = 49)
}
#endif
