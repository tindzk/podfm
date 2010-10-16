#import <Logger.h>

#import "Cache.h"
#import "Podcast.h"
#import "Storage.h"
#import "Debugger.h"
#import "Downloader.h"

typedef struct {
	String id;
	size_t size;

	void (*init)       (GenericClass this);
	void (*destroy)    (GenericClass this);
	void (*destroyItem)(void *item);
	void (*getListing) (GenericClass this, String name, Listing *res);
	void (*fetch)      (GenericClass this, DownloaderClass dl, Podcast item);
} ProviderInterface;

#undef self
#define self Provider

typedef struct {
	String  name;
	ssize_t limit;
	bool    inclDate;

	Logger            *logger;
	StringArray       *sources;
	ProviderInterface *methods;
	GenericClass       context;
	StorageClass       storage;
} Class(self);

def(void, Init, StorageClass storage, ProviderInterface *itf);
def(void, Destroy);
def(String, GetName);
def(void, SetName, String name);
def(void, SetLimit, ssize_t limit);
def(void, SetInclDate, bool value);
def(void, AddSource, String source);
def(void, Retrieve);
