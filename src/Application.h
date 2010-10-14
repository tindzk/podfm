#import <Block.h>
#import <String.h>
#import <Logger.h>

#import "Utils.h"
#import "Storage.h"
#import "Providers/RFI.h"
#import "Providers/RSS.h"

#undef self
#define self Application

typedef struct {
	ProviderInfo  *item;
	ProviderClass provider;
} ProviderInstance;

typedef struct {
	Logger *logger;
	StorageClass storage;

	Array(ProviderInstance, *providers);
} Class(self);

def(void, Init, Logger *logger, StorageClass storage);
def(void, Destroy);
def(StorageClass, GetStorage);
def(ProviderClass, AddProvider, String id);
def(void, Retrieve);
