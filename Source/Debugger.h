#import <Logger.h>
#import <String.h>
#import <Terminal.h>
#import <Terminal/Controller.h>

#define self Debugger

class {
	Logger logger;
};

def(Logger *, GetLogger);
SingletonPrototype(self);

#undef self
