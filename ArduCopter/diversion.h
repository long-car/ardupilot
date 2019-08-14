#ifndef DIVERSION_H
#define DIVERSION_H

#include "Copter.h"

class Diversion {
public:
	void divert (AP_Mission::Mission_Command &);
private:
	void print_mission ();
};

#endif