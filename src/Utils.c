#import "Utils.h"

extern Terminal term;

void Utils_OnLogMessage(__unused Logger *this, String msg, Logger_Level level, String file, int line) {
	String color  = String("black");
	String slevel = Logger_LevelToString(level);
	String sline = Integer_ToString(line);

	if (level == Logger_Level_Fatal || level == Logger_Level_Crit || level == Logger_Level_Error) {
		color = String("red");
	} else if (level == Logger_Level_Warn || level == Logger_Level_Info) {
		color = String("yellow");
	} else if (level == Logger_Level_Debug || level == Logger_Level_Trace) {
		color = String("cyan");
	}

	Terminal_Controller controller;
	Terminal_Controller_Init(&controller, &term);
	Terminal_Controller_Render(&controller,
		$(".fg[%]{.b{[%]} % .i{(%:%)}}\n"),
		color, slevel, msg, file, sline);
}
