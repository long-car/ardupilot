#include "Copter.h"
/*
bool Diversion::start_cmd (const AP_Mission::Mission_Command& cmd) {
	hal.console->printf("Starting command...\n");
    hal.console->printf("started cmd #%d id:%d Nav\n",(int)cmd.index,(int)cmd.id);

    return true;
}

bool Diversion::verify_cmd (const AP_Mission::Mission_Command& cmd) {
	hal.console->printf("Verifying command...\n");
	Location currLocation;
	AP::ahrs().get_position(currLocation);
	hal.console->printf("Change lattitude: %lu\n", cmd.content.location.lat-currLocation.lat);
	hal.console->printf("Change longitude: %lu\n", cmd.content.location.lng-currLocation.lng);
	if (currLocation.lat == cmd.content.location.lat && currLocation.lng == cmd.content.location.lng) return true;

    return false;
}

void Diversion::mission_complete (void) {
	complete = true;
    hal.console->printf("\nMission Complete!\n");
}*/

void Diversion::init_mission () {
    AP_Mission::Mission_Command cmd;

    copter.mode_auto.mission.clear();

    hal.console->printf("Adding commands...\n");
    // Command #0 : home
    cmd.id = MAV_CMD_NAV_WAYPOINT;
    cmd.content.location = Location{
        0,
        0,
        0,
        Location::AltFrame::ABOVE_HOME
    };
    if (!copter.mode_auto.mission.add_cmd(cmd)) {
        hal.console->printf("failed to add command\n");
    }

    // Command #1 : take-off to 10m
    cmd.id = MAV_CMD_NAV_TAKEOFF;
    cmd.p1 = 0;
    cmd.content.location = Location{
        0,
        0,
        100,
        Location::AltFrame::ABOVE_HOME
    };
    if (!copter.mode_auto.mission.add_cmd(cmd)) {
        hal.console->printf("failed to add command\n");
    }

    // Command #2 : first waypoint
    cmd.id = MAV_CMD_NAV_WAYPOINT;
    cmd.p1 = 0;
    cmd.content.location = Location{
        296718041,
        -986694669,
        100,
        Location::AltFrame::ABOVE_HOME
    };
    if (!copter.mode_auto.mission.add_cmd(cmd)) {
        hal.console->printf("failed to add command\n");
    }

    // Command #3 : second waypoint
    cmd.id = MAV_CMD_NAV_WAYPOINT;
    cmd.p1 = 0;
    cmd.content.location = Location{
        296720470,
        -986697057,
        100,
        Location::AltFrame::ABOVE_HOME
    };
    if (!copter.mode_auto.mission.add_cmd(cmd)) {
        hal.console->printf("failed to add command\n");
    }

    // Command #4 : do-jump to first waypoint 3 times
    cmd.id = MAV_CMD_NAV_WAYPOINT;
    cmd.p1 = 0;
    cmd.content.location = Location{
        296717426,
        -986698332,
        100,
        Location::AltFrame::ABOVE_HOME
    };
    if (!copter.mode_auto.mission.add_cmd(cmd)) {
        hal.console->printf("failed to add command\n");
    }

    // Command #5 : RTL
    cmd.id = MAV_CMD_NAV_RETURN_TO_LAUNCH;
    cmd.p1 = 0;
    cmd.content.location = Location{
        0,
        0,
        0,
        Location::AltFrame::ABOVE_HOME
    };
    if (!copter.mode_auto.mission.add_cmd(cmd)) {
        hal.console->printf("failed to add command\n");
    }
}

void Diversion::print_mission () {
	AP_Mission::Mission_Command cmd;

    // check for empty mission
    if (copter.mode_auto.mission.num_commands() == 0) {
        hal.console->printf("No Mission!\n");
        return;
    }

    hal.console->printf("Mission: %d commands\n",(int)copter.mode_auto.mission.num_commands());

    // print each command
    for(uint16_t i=0; i<copter.mode_auto.mission.num_commands(); i++) {
        // get next command from eeprom
        copter.mode_auto.mission.read_cmd_from_storage(i,cmd);

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
		}
	}
}

void Diversion::divert () {
	AP_Mission::Mission_Command cmd;

	cmd.id = MAV_CMD_NAV_WAYPOINT;
	cmd.p1 = 0;
	cmd.content.location = Location {
		296716115,
		-986696891,
		100,
		Location::AltFrame::ABOVE_HOME
	};

	hal.console->printf("Diverting...\n");
	uint16_t curr_index = copter.mode_auto.mission.get_current_nav_index();
	uint16_t start_index = curr_index;
	AP_Mission::Mission_Command nxtCmd;
	while (copter.mode_auto.mission.get_next_nav_cmd(curr_index, nxtCmd)) {
		if (!copter.mode_auto.mission.replace_cmd(curr_index, cmd)) hal.console->printf("Failed to replace command\n");
		cmd = nxtCmd;
		curr_index++;
	}
	if (!copter.mode_auto.mission.add_cmd(cmd)) hal.console->printf("Failed to add command\n");

	hal.console->printf("Updated commands...\n");
	hal.console->printf("Restarting...\n");
	copter.mode_auto.mission.reset();
	print_mission();
	if (!copter.mode_auto.mission.set_current_cmd(start_index)) hal.console->printf("Could not restart\n");
}

void running (Diversion &dv) {
	dv.runit();
}

void divertit (Diversion &dv) {
	dv.divert();
}