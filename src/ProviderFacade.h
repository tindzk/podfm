#import <Logger.h>

#import "Cache.h"
#import "Podcast.h"
#import "Storage.h"
#import "Debugger.h"
#import "Downloader.h"
#import "ProviderInterface.h"

#undef self
#define self ProviderFacade

class {
	String name;
	s32    limit;

	Logger            *logger;
	StringArray       *sources;
	ProviderInterface *provider;
	GenericInstance   instance;
	StorageInstance   storage;
};

ExtendClass;

def(void, Init, StorageInstance storage, ProviderInterface *provider);
def(void, Destroy);
def(String, GetName);
def(void, SetName, String name);
def(void, SetLimit, s32 limit);
def(bool, SetOption, String key, String value);
def(void, AddSource, String source);
def(void, Retrieve);
