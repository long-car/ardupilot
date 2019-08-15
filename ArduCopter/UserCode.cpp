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
    userTimer++;	// Update timer

    // Trigger diversion after 150 seconds
    if (userTimer == 30) {
		cmd.id = MAV_CMD_NAV_WAYPOINT;	// Command ID set to waypoint navigation
		cmd.p1 = 1;	// User controlled value set to zero because it's useless
		cmd.content.location = Location {	// Assign location info for waypoint
			296719348,	//	latitude
			-986691232,	// 	longitude
			100,		//	altitude in cm
			Location::AltFrame::ABOVE_HOME	// altitude reference fram set to home
		};
		cmd_lst[0] = cmd;	// add command to cmd_lst array as first wp in diversion

		// update cmd location to second waypoint destination
		cmd.content.location = Location {
			296720575,
			-986692617,
			100,
			Location::AltFrame::ABOVE_HOME
		};
		cmd_lst[1] = cmd;	// add command to list

		// update cmd location to third waypoint destination
		cmd.content.location = Location {
			296719987,
			-986694616,
			100,
			Location::AltFrame::ABOVE_HOME
		};
		cmd_lst[2] = cmd;	// add command to list

		// call divert to update mission with new commands from cmd_lst starting at waypoint 6
    	if (!Diversion().divert(cmd_lst, 3, 6)) hal.console->printf("Failed to Divert\n");
    }
    if (userTimer == 60) {
    	cmd.p1 = 2;
    	cmd.content.location = Location {	// Assign location info for waypoint
			296719845,	//	latitude
			-986692265,	// 	longitude
			100,		//	altitude in cm
			Location::AltFrame::ABOVE_HOME	// altitude reference fram set to home
		};
		cmd_lst[0] = cmd;	// add command to cmd_lst array as first wp in diversion

		// update cmd location to second waypoint destination
		cmd.content.location = Location {
			296719194,
			-986694668,
			100,
			Location::AltFrame::ABOVE_HOME
		};
		cmd_lst[1] = cmd;	// add command to list

		// update cmd location to third waypoint destination
		cmd.content.location = Location {
			296719559,
			-986696126,
			100,
			Location::AltFrame::ABOVE_HOME
		};
		cmd_lst[2] = cmd;	// add command to list

		if (!Diversion().replace(cmd_lst, 3, 6, 8)) hal.console->printf("Failed to Replace\n");
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
