#import <Block.h>
#import <Logger.h>
#import <String.h>

#import "Storage.h"
#import "Debugger.h"
#import "Providers.h"
#import "ProviderFacade.h"

#define self Application

Array(ProviderFacadeInstance, Providers);

class {
	StorageInstance storage;
	Providers *providers;
};

def(void, Init, StorageInstance storage);
def(void, Destroy);
def(StorageInstance, GetStorage);
def(ProviderFacadeInstance, AddProvider, String id);
def(void, Retrieve);

#undef self
