#ifndef DIVERSION_H
#define DIVERSION_H

#include <AP_Mission/AP_Mission.h>

// Class to access flight mission and command diversions
class Diversion {
public:
	// divert command for single command, automatically restarts mission from new command unless specified
	bool divert (AP_Mission::Mission_Command &, bool restart = 1, int start = -1);

	// divert command for a list of commands, specify number of commands as second param
	bool divert (AP_Mission::Mission_Command [], int, int start = -1);
private:
	// prints mission
	void print_mission ();
};

#endif