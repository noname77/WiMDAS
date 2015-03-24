//#include "tinysh.h"

#ifndef COMMANDS_H
#define	COMMANDS_H

extern void test_fnt (int argc, char **argv);
//extern tinysh_cmd_t testcmd;
extern uint8_t write_index;

void register_commands();

#endif	/* COMMANDS_H */

