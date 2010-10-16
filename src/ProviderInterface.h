#import <String.h>

#import "Podcast.h"
#import "Downloader.h"

typedef struct {
	String id;
	size_t size;

	void (*init)       (GenericInstance this);
	void (*destroy)    (GenericInstance this);
	void (*destroyItem)(void *item);
	void (*getListing) (GenericInstance this, String name, Listing *res);
	void (*fetch)      (GenericInstance this, DownloaderInstance dl, Podcast item);
} ProviderInterface;
