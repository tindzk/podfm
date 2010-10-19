#import <File.h>
#import <Path.h>
#import <String.h>
#import <Logger.h>

#import "Storage.h"
#import "Debugger.h"

#undef self
#define self Cache

enum {
	excIndexFileTooLarge = excOffset
};

class(self) {
	File file;
	StringArray *items;
};

ExtendClass(self);

void Cache0(ExceptionManager *e);

def(void, Init, StorageInstance storage, String provider);
def(void, Destroy);
def(bool, Has, String id);
def(void, Add, String id);
