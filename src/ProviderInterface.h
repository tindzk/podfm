#import <String.h>

#import "Podcast.h"
#import "Downloader.h"

record(ProviderInterface) {
	String id;
	size_t size;

	void (*init)       (GenericInstance $this);
	void (*destroy)    (GenericInstance $this);
	void (*destroyItem)(void *item);
	bool (*setOption)  (GenericInstance $this, String key, String value);
	void (*getListing) (GenericInstance $this, String name, Listing **res);
	void (*fetch)      (GenericInstance $this, DownloaderInstance dl, ListingItem *item);
};
