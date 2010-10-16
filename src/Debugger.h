#import <Logger.h>
#import <String.h>
#import <Terminal.h>
#import <Terminal/Controller.h>

#undef self
#define self Debugger

record {
	Logger logger;
} Class(Debugger);

def(Logger *, GetLogger);
DebuggerInstance ref(GetInstance)(void);
