#import <Block.h>
#import <Logger.h>
#import <String.h>

#import "Storage.h"
#import "Debugger.h"
#import "Providers.h"
#import "ProviderFacade.h"

#undef self
#define self Application

Array_Define(ProviderFacadeInstance, Providers);

class(self) {
	StorageInstance storage;
	Providers *providers;
};

ExtendClass(self);

def(void, Init, StorageInstance storage);
def(void, Destroy);
def(StorageInstance, GetStorage);
def(ProviderFacadeInstance, AddProvider, String id);
def(void, Retrieve);
