#ifndef DIVERSION_H
#define DIVERSION_H

#include "Copter.h"

class Diversion {
public:
	void runit ();
private:
	uint8_t verify_nav_cmd_iterations_to_complete = 3;
    uint8_t verify_do_cmd_iterations_to_complete = 1;
    uint8_t num_nav_cmd_runs = 0;
    uint8_t num_do_cmd_runs = 0;
    bool mission_setup = false;
    bool complete = false;

	bool start_cmd (const AP_Mission::Mission_Command& cmd);
	bool verify_cmd (const AP_Mission::Mission_Command& cmd);
	void mission_complete (void);
	void init_mission ();
	void print_mission ();

	AP_Mission mission{
		FUNCTOR_BIND_MEMBER(&Diversion::start_cmd, bool, const AP_Mission::Mission_Command &),
		FUNCTOR_BIND_MEMBER(&Diversion::verify_cmd, bool, const AP_Mission::Mission_Command &),
		FUNCTOR_BIND_MEMBER(&Diversion::mission_complete, void)};
};

void running (Diversion &);

#endif