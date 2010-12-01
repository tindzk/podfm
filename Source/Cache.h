#import <File.h>
#import <Path.h>
#import <String.h>
#import <Logger.h>

#import "Storage.h"
#import "Debugger.h"

#define self Cache

enum {
	excIndexFileTooLarge = excOffset
};

class {
	File file;
	StringArray *items;
};

def(void, Init, StorageInstance storage, String provider);
def(void, Destroy);
def(bool, Has, String id);
def(void, Add, String id);

#undef self
