#import <Logger.h>
#import <String.h>
#import <Terminal.h>
#import <Terminal/Controller.h>

#undef self
#define self Debugger

Class(self) {
	Logger logger;
};

def(Logger *, GetLogger);
DebuggerInstance ref(GetInstance)(void);
