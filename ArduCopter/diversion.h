#ifndef DIVERSION_H
#define DIVERSION_H

#include "Copter.h"

class Diversion {
public:
	void runit ();
	void divert ();
private:
    bool mission_setup = false;
    bool complete = false;
    bool diverted = false;

	void init_mission ();
	void print_mission ();
};

void running (Diversion &);
void divertit (Diversion &);

#endif