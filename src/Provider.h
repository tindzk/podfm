#import <Logger.h>

#import "Cache.h"
#import "Podcast.h"
#import "Storage.h"
#import "Downloader.h"

typedef struct {
	size_t size;

	void (*init)       (GenericClass this, Logger *logger);
	void (*destroy)    (GenericClass this);
	void (*destroyItem)(void *item);
	void (*getListing) (GenericClass this, String name, Listing *res);
	void (*fetch)      (GenericClass this, DownloaderClass dl, Podcast item);
} ProviderInterface;

typedef struct {
	String id;
	ProviderInterface methods;
} ProviderInfo;

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

def(void, Init, Logger *logger, StorageClass storage, ProviderInterface *itf, String providerId);
def(void, Destroy);
def(String, GetName);
def(void, SetName, String name);
def(void, SetLimit, ssize_t limit);
def(void, SetInclDate, bool value);
def(void, AddSource, String source);
def(void, Retrieve);
