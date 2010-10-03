#import <Block.h>
#import <String.h>
#import <Logger.h>

#import "Utils.h"
#import "Storage.h"
#import "Providers/RFI.h"

#undef self
#define self Application

typedef struct {
	ProviderInfo *item;
	Provider     *provider;
} ProviderInstance;

typedef struct {
	Logger  *logger;
	Storage *storage;

	Array(ProviderInstance, *providers);
} self;

def(void, Init, Logger *logger, Storage *storage);
def(void, Destroy);
def(Storage *, GetStorage);
def(Provider *, AddProvider, String id);
def(void, Retrieve);
