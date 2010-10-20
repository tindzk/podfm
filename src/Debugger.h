#import <Logger.h>
#import <String.h>
#import <Terminal.h>
#import <Terminal/Controller.h>

#undef self
#define self Debugger

class(self) {
	Logger logger;
};

def(Logger *, GetLogger);
SingletonPrototype(self);
