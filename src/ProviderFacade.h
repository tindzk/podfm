#import <Logger.h>

#import "Cache.h"
#import "Podcast.h"
#import "Storage.h"
#import "Debugger.h"
#import "Downloader.h"
#import "ProviderInterface.h"

#undef self
#define self ProviderFacade

Class(self) {
	String  name;
	ssize_t limit;
	bool    inclDate;

	Logger            *logger;
	StringArray       *sources;
	ProviderInterface *methods;
	GenericInstance   context;
	StorageInstance   storage;
};

ExtendClass(self);

def(void, Init, StorageInstance storage, ProviderInterface *itf);
def(void, Destroy);
def(String, GetName);
def(void, SetName, String name);
def(void, SetLimit, ssize_t limit);
def(void, SetInclDate, bool value);
def(void, AddSource, String source);
def(void, Retrieve);
