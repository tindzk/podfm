#import <YAML.h>
#import <Path.h>
#import <String.h>
#import <Logger.h>
#import <FileStream.h>
#import <BufferedStream.h>

#import "Application.h"

#undef self
#define self Configuration

typedef struct {
	Logger *logger;
	ApplicationClass app;
} Class(self);

def(void, Init, ApplicationClass app, Logger *logger);
def(void, Parse);
