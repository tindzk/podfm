#import <Logger.h>
#import <String.h>
#import <Terminal.h>
#import <Terminal/Controller.h>

#undef self
#define self Debugger

class {
	Logger logger;
};

def(Logger *, GetLogger);
SingletonPrototype(self);
