#include "diversion.h"

bool Diversion::start_cmd (const AP_Mission::Mission_Command& cmd) {
	hal.console->printf("Starting command...\n");
    if (AP_Mission::is_nav_cmd(cmd)) {
        num_nav_cmd_runs = 0;
        hal.console->printf("started cmd #%d id:%d Nav\n",(int)cmd.index,(int)cmd.id);
    }else{
        num_do_cmd_runs = 0;
        hal.console->printf("started cmd #%d id:%d Do\n",(int)cmd.index,(int)cmd.id);
    }

    return true;
}

bool Diversion::verify_cmd (const AP_Mission::Mission_Command& cmd) {
	hal.console->printf("Verifying command...\n");
    if (AP_Mission::is_nav_cmd(cmd)) {
        num_nav_cmd_runs++;
        if (num_nav_cmd_runs < verify_nav_cmd_iterations_to_complete) {
            hal.console->printf("verified cmd #%d id:%d Nav iteration:%d\n",(int)cmd.index,(int)cmd.id,(int)num_nav_cmd_runs);
            return false;
        }else{
            hal.console->printf("verified cmd #%d id:%d Nav complete!\n",(int)cmd.index,(int)cmd.id);
            return true;
        }
    }else{
        num_do_cmd_runs++;
        if (num_do_cmd_runs < verify_do_cmd_iterations_to_complete) {
            hal.console->printf("verified cmd #%d id:%d Do iteration:%d\n",(int)cmd.index,(int)cmd.id,(int)num_do_cmd_runs);
            return false;
        }else{
            hal.console->printf("verified cmd #%d id:%d Do complete!\n",(int)cmd.index,(int)cmd.id);
            return true;
        }
    }
    return true;
}

void Diversion::mission_complete (void) {
	complete = true;
    hal.console->printf("\nMission Complete!\n");
}

void Diversion::init_mission () {
    AP_Mission::Mission_Command cmd;

    mission.clear();

    hal.console->printf("Adding commands...\n");
    // Command #0 : home
    cmd.id = MAV_CMD_NAV_WAYPOINT;
    cmd.content.location = Location{
        12345678,
        23456789,
        0,
        Location::AltFrame::ABSOLUTE
    };
    if (!mission.add_cmd(cmd)) {
        hal.console->printf("failed to add command\n");
    }

    // Command #1 : take-off to 10m
    cmd.id = MAV_CMD_NAV_TAKEOFF;
    cmd.p1 = 0;
    cmd.content.location = Location{
        0,
        0,
        10,
        Location::AltFrame::ABSOLUTE
    };
    if (!mission.add_cmd(cmd)) {
        hal.console->printf("failed to add command\n");
    }

    // Command #2 : first waypoint
    cmd.id = MAV_CMD_NAV_WAYPOINT;
    cmd.p1 = 0;
    cmd.content.location = Location{
        12345678,
        23456789,
        11,
        Location::AltFrame::ABSOLUTE
    };
    if (!mission.add_cmd(cmd)) {
        hal.console->printf("failed to add command\n");
    }

    // Command #3 : second waypoint
    cmd.id = MAV_CMD_NAV_WAYPOINT;
    cmd.p1 = 0;
    cmd.content.location = Location{
        1234567890,
        -1234567890,
        22,
        Location::AltFrame::ABSOLUTE
    };
    if (!mission.add_cmd(cmd)) {
        hal.console->printf("failed to add command\n");
    }

    // Command #4 : do-jump to first waypoint 3 times
    cmd.id = MAV_CMD_DO_JUMP;
    cmd.content.jump.target = 2;
    cmd.content.jump.num_times = 1;
    if (!mission.add_cmd(cmd)) {
        hal.console->printf("failed to add command\n");
    }

    // Command #5 : RTL
    cmd.id = MAV_CMD_NAV_RETURN_TO_LAUNCH;
    cmd.p1 = 0;
    cmd.content.location = Location{
        0,
        0,
        0,
        Location::AltFrame::ABSOLUTE
    };
    if (!mission.add_cmd(cmd)) {
        hal.console->printf("failed to add command\n");
    }
}

void Diversion::print_mission () {
	AP_Mission::Mission_Command cmd;

    // check for empty mission
    if (mission.num_commands() == 0) {
        hal.console->printf("No Mission!\n");
        return;
    }

    hal.console->printf("Mission: %d commands\n",(int)mission.num_commands());

    // print each command
    for(uint16_t i=0; i<mission.num_commands(); i++) {
        // get next command from eeprom
        mission.read_cmd_from_storage(i,cmd);

        // print command position in list and mavlink id
        hal.console->printf("Cmd#%d mav-id:%d ", (int)cmd.index, (int)cmd.id);

        // print whether nav or do command
        if (AP_Mission::is_nav_cmd(cmd)) {
            hal.console->printf("Nav ");
        }else{
            hal.console->printf("Do ");
        }

        // print command contents
        if (cmd.id == MAV_CMD_DO_JUMP) {
            hal.console->printf("jump-to:%d num_times:%d\n", (int)cmd.content.jump.target, (int)cmd.content.jump.num_times);
        }else{
            hal.console->printf("p1:%d lat:%ld lng:%ld alt:%ld\n",(int)cmd.p1, (long)cmd.content.location.lat, (long)cmd.content.location.lng, (long)cmd.content.location.alt);
        }
    }
    hal.console->printf("--------\n");
}

void Diversion::runit () {
	if (!complete) {
		if (!mission_setup) {
			hal.console->printf("Initializing mission...\n");
			init_mission();
			
			hal.console->printf("Printing mission...\n");
			print_mission();

			mission_setup = true;
		    hal.console->printf("Mission Setup Complete.\n");

		    // start mission
		    hal.console->printf("Starting mission...\n");
		    mission.start();
		}

		hal.console->printf("Updating...\n");
		mission.update();
	}
}

void running (Diversion &dv) {
	dv.runit();
}

