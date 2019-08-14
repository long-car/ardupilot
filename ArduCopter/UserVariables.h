// user defined variables

// example variables used in Wii camera testing - replace with your own
// variables
#ifdef USERHOOK_VARIABLES

#if WII_CAMERA == 1
WiiCamera           ircam;
int                 WiiRange=0;
int                 WiiRotation=0;
int                 WiiDisplacementX=0;
int                 WiiDisplacementY=0;
#endif  // WII_CAMERA

int userTimer = 0;	// Timer to track number of seconds since startup to trigger diversion
AP_Mission::Mission_Command cmd;	// Mission command
AP_Mission::Mission_Command cmd_lst [3];	// Array of mission commands

#endif  // USERHOOK_VARIABLES


