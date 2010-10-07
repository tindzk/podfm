#import <Logger.h>

#import "Cache.h"
#import "Podcast.h"
#import "Storage.h"
#import "Downloader.h"

typedef struct {
	size_t size;
	void (*init)(void *this, Logger *logger);
	void (*destroy)(void *this);
	void (*destroyItem)(void *item);
	void (*getListing)(void *this, String name, Listing *res);
	void (*fetch)(void *this, Downloader *dl, Podcast item);
} ProviderInterface;

typedef struct {
	String id;
	ProviderInterface methods;
} ProviderInfo;

#undef self
#define self Provider

typedef struct {
	Logger      *logger;
	Storage     *storage;
	StringArray *sources;

	struct {
		ProviderInterface *methods;
		void *context;
	} backend;

	String name;
} self;

def(void, Init, Logger *logger, Storage *storage, ProviderInterface *itf, String providerId);
def(void, Destroy);
def(String, GetName);
def(void, SetName, String name);
def(void, AddSource, String source);
def(void, Retrieve);
