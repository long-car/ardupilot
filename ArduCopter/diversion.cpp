#include "Copter.h"

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

void Diversion::divert (AP_Mission::Mission_Command &cmd) {
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
