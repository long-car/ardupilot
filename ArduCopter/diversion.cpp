#include "Copter.h"

//	Prints mission to console
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

// Diverts single command, restarts mission from diverted command if restart == true
// Restart is automatic unless restart is set to 0
// Replaces command specified by start
// Returns false if diversion fails
bool Diversion::divert (AP_Mission::Mission_Command &cmd, int start, bool restart) {
	bool ret = true;

	hal.console->printf("Diverting...\n");

	// Mark current nav index
	uint16_t curr_index = copter.mode_auto.mission.get_current_nav_index();

	// Check to see where to start from
	uint16_t start_index = 0;
	if (start == -1) start_index = curr_index;
	else start_index = start;
	
	AP_Mission::Mission_Command nxtCmd;	// Command to hold next command

	//	Iterate through entire mission
	while (copter.mode_auto.mission.get_next_nav_cmd(start_index, nxtCmd)) {
		// Replace command at current index with updated command
		if (!copter.mode_auto.mission.replace_cmd(start_index, cmd)) {
			hal.console->printf("Failed to replace command\n");
			ret = false;
		}
		cmd = nxtCmd;
		start_index++;
	}
	// Add final command to end of mission
	if (!copter.mode_auto.mission.add_cmd(cmd)) {
		hal.console->printf("Failed to add command\n");
		ret = false;
	}

	// Restart mission from curr_index if restart is specified
	if (restart) {
		hal.console->printf("Updated commands...\n");
		hal.console->printf("Restarting...\n");
		copter.mode_auto.mission.reset();
		print_mission();	// Print updated mission
		// Restart mission from curr_index
		if (!copter.mode_auto.mission.set_current_cmd(curr_index)) {
			hal.console->printf("Could not restart\n");
			ret = false;
		}
	}

	return ret;	// Returns true if diversion succeeds
}

// Updates mission with list of new diversions starting at index start
bool Diversion::divert (AP_Mission::Mission_Command cmd[], int num_cmds, int start, bool restart) {
	bool ret = true;

	hal.console->printf("Diverting %d Commands...\n", num_cmds);

	// Force restart if start is at current index
	if (start == -1) restart = 1;

	// Update mission with each new command, starting with last command
	for (int i = num_cmds-1; i >= 0; i--) {
		if (i != 0) {
			if (!divert(cmd[i], start, 0)) {	// Don't restart until all commands are updated
				hal.console->printf("Failed to divert\n");
				ret = false;
			}
		} else {	// Restart mission after adding final command
			if (!divert(cmd[i], start, restart)) {
				hal.console->printf("Failed to divert\n");
				ret = false;
			}
		}
	}

	return ret;	// Returns true if diversion is successful
}

// Replaces specified range of commands with single command
bool Diversion::replace (AP_Mission::Mission_Command &cmd, int start, int end) {
	bool ret = true;

	// Check for valid start and end indices
	if (start > end) return false;
	if (end >= copter.mode_auto.mission.num_commands()) return false;
	if (start < 0) return false;

	hal.console->printf("Replacing...\n");

	// Save current nav index
	uint16_t curr_index = copter.mode_auto.mission.get_current_nav_index();

	// Get starting idex to replace
	uint16_t start_index = 0;
	if (start == -1 || start < curr_index) start_index = curr_index;
	else start_index = start;

	// Save all commands that will not be replaced
	int saved_cmds = 0;
	AP_Mission::Mission_Command nxtCmd;	// Command to hold next command
	if (start != -1 && end != -1) {
		saved_cmds = (start_index - curr_index) + 1 + (copter.mode_auto.mission.num_commands() - end);
		AP_Mission::Mission_Command cmds [saved_cmds];
		cmds[0] = copter.mode_auto.mission.get_current_nav_cmd();
		int j = 1;
		for (uint16_t i = curr_index; i < start_index; i++) {
			copter.mode_auto.mission.get_next_nav_cmd(i, nxtCmd);
			cmds[j] = nxtCmd;
			j++;
		}
		cmds[j] = cmd;
		j++;
		for (int i = end+1; i < copter.mode_auto.mission.num_commands(); i++) {
			copter.mode_auto.mission.get_next_nav_cmd(i, nxtCmd);
			cmds[j] = nxtCmd;
			j++;
		}

		// Delete all commands going forward
		hal.console->printf("Resetting mission...\n");
		copter.mode_auto.mission.reset();
		copter.mode_auto.mission.truncate(curr_index-1);

		// Add new commands back to list
		for (int i = 0; i < saved_cmds; i++) {
			if (!copter.mode_auto.mission.add_cmd(cmds[i])) {
				hal.console->printf("Failed to add command\n");
				ret = false;
			}
		}
		if (!copter.mode_auto.mission.set_current_cmd(curr_index)) {
			hal.console->printf("Failed to reset\n");
			ret = false;
		}
	} else {	// just clear the rest of the mission and add cmd
		hal.console->printf("Resetting mission...\n");
		copter.mode_auto.mission.reset();
		copter.mode_auto.mission.truncate(curr_index-1);
		if (!copter.mode_auto.mission.add_cmd(cmd)) {
			hal.console->printf("Failed to add command\n");
			ret = false;
		}
		if (!copter.mode_auto.mission.set_current_cmd(curr_index)) {
			hal.console->printf("Failed to reset\n");
			ret = false;
		}
	}

	return ret;
}

// Replaces specified portion with list of commands
bool Diversion::replace (AP_Mission::Mission_Command cmd[], int num_cmds, int start, int end) {
	bool ret = true;

	hal.console->printf("Replacing %d Commands...\n", num_cmds);

	// Update mission with each new command, starting with last command
	for (int i = num_cmds-1; i >= 0; i--) {
		if (i == num_cmds-1) {
			if (!replace(cmd[i], start, end)) {
				hal.console->printf("Failed to replace\n");
			}
		}
		else if (i != 0) {
			if (!divert(cmd[i], start, 0)) {
				hal.console->printf("Failed to replace\n");
				ret = false;
			}
		} else {	// Restart mission after adding final command
			if (!divert(cmd[i], start)) {
				hal.console->printf("Failed to replace\n");
				ret = false;
			}
		}
	}

	return ret;
}



