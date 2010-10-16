#import <File.h>
#import <Path.h>
#import <String.h>
#import <Logger.h>
#import <StringArray.h>

#import "Storage.h"
#import "Debugger.h"

#undef self
#define self Cache

enum {
	excIndexFileTooLarge = excOffset
};

typedef struct {
	File file;
	StringArray *items;
} Class(self);

void Cache0(ExceptionManager *e);

def(void, Init, StorageClass storage, String provider);
def(void, Destroy);
def(bool, Has, String id);
def(void, Add, String id);
