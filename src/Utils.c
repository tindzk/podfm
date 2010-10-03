#import "Utils.h"

extern Terminal term;

void Utils_OnLogMessage(__unused Logger *this, String msg,
	Logger_Level level, __unused String file, __unused int line)
{
	String slevel = Logger_LevelToString(level);

	String fmt = String_Format($("[%] %\n"), slevel, msg);

	if (level == Logger_Level_Error || level == Logger_Level_Fatal) {
		Terminal_Print(&term,
			Terminal_Color_ForegroundRed,
			Terminal_Font_Normal,
			fmt);
	} else if (level == Logger_Level_Info) {
		Terminal_Print(&term,
			Terminal_Color_ForegroundCyan,
			Terminal_Font_Normal,
			fmt);
	} else if (level == Logger_Level_Debug) {
		Terminal_Print(&term,
			Terminal_Color_ForegroundYellow,
			Terminal_Font_Normal,
			fmt);
	} else {
		Terminal_Print(&term,
			Terminal_Color_Normal,
			Terminal_Font_Normal,
			fmt);
	}

	String_Destroy(&fmt);
}
