#import "Debugger.h"
#import <App.h>

extern Terminal term;

static def(void, Init);
static def(void, OnLogMessage, String msg, Logger_Level level, String file, int line);

/* Design pattern: Singleton */
DebuggerClass ref(GetClass)(void) {
	static Debugger      storage;
	static DebuggerClass class;

	if (Debugger_IsNull(class)) {
		class = ref(AsClass)(&storage);
		ref(Init)(class);
	}

	return class;
}

static def(void, Init) {
	Logger_Init(&this->logger, (void *) ref(OnLogMessage), this,
		Logger_Level_Fatal |
		Logger_Level_Crit  |
		Logger_Level_Error |
		Logger_Level_Warn  |
		Logger_Level_Info  |
		Logger_Level_Debug |
		Logger_Level_Trace);
}

static def(void, OnLogMessage, String msg, Logger_Level level, String file, int line) {
	String color  = $("black");
	String slevel = Logger_LevelToString(level);
	String sline  = Integer_ToString(line);

	if (level == Logger_Level_Fatal || level == Logger_Level_Crit || level == Logger_Level_Error) {
		color = $("red");
	} else if (level == Logger_Level_Warn || level == Logger_Level_Info) {
		color = $("yellow");
	} else if (level == Logger_Level_Debug || level == Logger_Level_Trace) {
		color = $("cyan");
	}

	Terminal_Controller controller;
	Terminal_Controller_Init(&controller, &term);
	Terminal_Controller_Render(&controller,
		$(".fg[%]{.b{[%]} % .i{(%:%)}}\n"),
		color, slevel, msg, file, sline);
}

def(Logger *, GetLogger) {
	return &this->logger;
}
