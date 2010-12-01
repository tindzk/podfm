#import <YAML.h>
#import <Path.h>
#import <String.h>
#import <Logger.h>
#import <FileStream.h>
#import <BufferedStream.h>

#import "Debugger.h"
#import "Application.h"

#undef self
#define self Configuration

class {
	Logger *logger;
	ApplicationInstance app;
};

def(void, Init, ApplicationInstance app);
def(void, Parse);
