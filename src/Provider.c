#import "Provider.h"

#import <App.h>

def(void, Init, Logger *logger, StorageClass storage, ProviderInterface *itf, String providerId) {
	this->logger          = logger;
	this->storage         = storage;
	this->name            = String_Clone(providerId);
	this->limit           = -1;
	this->inclDate        = true;

	Array_Init(this->sources, 10);

	this->methods = itf;
	this->context = Generic_New(itf->size);
	this->methods->init(this->context, this->logger);
}

def(void, Destroy) {
	this->methods->destroy(this->context);
	Generic_Free(this->context);

	String_Destroy(&this->name);

	Array_Foreach(this->sources, String_Destroy);
	Array_Destroy(this->sources);
}

def(String, GetName) {
	return String_Disown(this->name);
}

def(void, SetName, String name) {
	String_Copy(&this->name, name);
}

def(void, SetLimit, ssize_t limit) {
	this->limit = limit;
}

def(void, SetInclDate, bool value) {
	this->inclDate = value;
}

def(void, AddSource, String source) {
	Array_Push(this->sources, String_Clone(source));
}

def(void, Retrieve) {
	struct {
		Cache      cache;
		Downloader dl;
	} private;

	DownloaderClass dl = Downloader_AsClass(&private.dl);
	Downloader_Init(dl, this->storage, this->logger, this->name);
	Downloader_SetInclDate(dl, this->inclDate);

	CacheClass cache = Cache_AsClass(&private.cache);
	Cache_Init(cache, this->storage, this->logger, this->name);

	for (size_t i = 0; i < this->sources->len; i++) {
		String source = this->sources->buf[i];

		Logger_LogFmt(this->logger, Logger_Level_Info,
			$("Requesting listing for source '%'..."), source);

		Listing *listing;
		Array_Init(listing, 128);

		this->methods->getListing(this->context, source, listing);

		Logger_LogFmt(this->logger, Logger_Level_Info,
			$("% items found (limit=%)."),
			Integer_ToString(listing->len),
			(this->limit == -1)
				? $("no")
				: Integer_ToString(this->limit));

		for (size_t j = 0; j < listing->len; j++) {
			if (this->limit >= 0 && j >= (size_t) this->limit) {
				goto next;
			}

			if (listing->buf[j].id.len == 0) {
				Logger_LogFmt(this->logger, Logger_Level_Error,
					$("[%/%] The ID is empty!"),
					Integer_ToString(j + 1),
					Integer_ToString(listing->len));

				goto next;
			}

			Logger_LogFmt(this->logger, Logger_Level_Info,
				$("[%/%] Fetching podcast '%' (%)..."),
				Integer_ToString(j + 1),
				Integer_ToString(listing->len),
				listing->buf[j].title,
				listing->buf[j].id);

			if (Cache_Has(cache, listing->buf[j].id)) {
				Logger_Log(this->logger, Logger_Level_Info,
					$("Already in cache. Skipping..."));

				goto next;
			}

			this->methods->fetch(this->context, dl,
				listing->buf[j]);

			Cache_Add(cache, listing->buf[j].id);

		next:
			String_Destroy(&listing->buf[j].id);
			String_Destroy(&listing->buf[j].title);
			this->methods->destroyItem(listing->buf[j].data);
			Memory_Free(listing->buf[j].data);
		}

		Array_Destroy(listing);
	}

	Downloader_Destroy(dl);

	Cache_Destroy(cache);
}
