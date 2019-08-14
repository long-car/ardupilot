#ifndef DIVERSION_H
#define DIVERSION_H

#include <AP_Mission/AP_Mission.h>

class Diversion {
public:
	void divert (AP_Mission::Mission_Command &);
private:
	void print_mission ();
};

#endif