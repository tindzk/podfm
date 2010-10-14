#import <File.h>
#import <Path.h>
#import <String.h>
#import <Logger.h>
#import <StringArray.h>

#import "Utils.h"
#import "Storage.h"

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

def(void, Init, StorageClass storage, Logger *logger, String provider);
def(void, Destroy);
def(bool, Has, String id);
def(void, Add, String id);
