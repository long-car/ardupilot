#ifndef DIVERSION_H
#define DIVERSION_H

#include <AP_Mission/AP_Mission.h>

class Diversion {
public:
	bool divert (AP_Mission::Mission_Command &, bool restart = 1);
	bool divert (AP_Mission::Mission_Command [], int);
private:
	void print_mission ();
};

#endif