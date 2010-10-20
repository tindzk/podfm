#import "ProviderFacade.h"
#import <App.h>

extern ExceptionManager exc;

def(void, Init, StorageInstance storage, ProviderInterface *itf) {
	this->logger   = Debugger_GetLogger(Debugger_GetInstance());
	this->storage  = storage;
	this->name     = String_Clone(itf->id);
	this->limit    = -1;
	this->inclDate = true;
	this->sources  = StringArray_New(10);

	this->methods = itf;

	if (itf->size > 0) {
		this->context = Generic_New(itf->size);
	} else {
		this->context = Generic_Null();
	}

	if (this->methods->init != NULL) {
		this->methods->init(this->context);
	}
}

def(void, Destroy) {
	if (this->methods->destroy != NULL) {
		this->methods->destroy(this->context);
	}

	if (!Generic_IsNull(this->context)) {
		Generic_Free(this->context);
	}

	String_Destroy(&this->name);

	Array_Foreach(this->sources, String_Destroy);
	StringArray_Free(this->sources);
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

static def(void, DestroyPodcast, Podcast *podcast) {
	String_Destroy(&podcast->id);
	String_Destroy(&podcast->title);
	this->methods->destroyItem(podcast->data);
	Memory_Free(podcast->data);
}

static def(void, Fetch, DownloaderInstance dl, CacheInstance cache, Listing *listing) {
	for (size_t i = 0; i < listing->len; i++) {
		if (this->limit >= 0 && i >= (size_t) this->limit) {
			call(DestroyPodcast, &listing->buf[i]);
			continue;
		}

		if (listing->buf[i].id.len == 0) {
			Logger_Error(this->logger,
				$("[%/%] The ID is empty!"),
				Integer_ToString(i + 1),
				Integer_ToString(listing->len));

			call(DestroyPodcast, &listing->buf[i]);
			continue;
		}

		Logger_Info(this->logger,
			$("[%/%] Fetching podcast '%' (%)..."),
			Integer_ToString(i + 1),
			Integer_ToString(listing->len),
			listing->buf[i].title,
			listing->buf[i].id);

		if (Cache_Has(cache, listing->buf[i].id)) {
			Logger_Info(this->logger,
				$("Already in cache. Skipping..."));

			call(DestroyPodcast, &listing->buf[i]);
			continue;
		}

		try (&exc) {
			this->methods->fetch(this->context, dl,
				listing->buf[i]);

			Cache_Add(cache, listing->buf[i].id);
		} clean finally {
			call(DestroyPodcast, &listing->buf[i]);
		} tryEnd;
	}
}

static def(void, Request, DownloaderInstance dl, CacheInstance cache) {
	for (size_t i = 0; i < this->sources->len; i++) {
		String source = this->sources->buf[i];

		Logger_Info(this->logger,
			$("Requesting listing for source '%'..."), source);

		Listing *listing = Listing_New(128);

		try (&exc) {
			this->methods->getListing(this->context, source, &listing);

			Logger_Info(this->logger, $("% items found (limit=%)."),
				Integer_ToString(listing->len),
				(this->limit == -1)
					? $("no")
					: Integer_ToString(this->limit));

			call(Fetch, dl, cache, listing);
		} clean finally {
			Listing_Free(listing);
		} tryEnd;
	}
}

def(void, Retrieve) {
	DownloaderInstance dl = Downloader_NewStack();
	Downloader_Init(dl, this->storage, this->name);
	Downloader_SetInclDate(dl, this->inclDate);

	CacheInstance cache = Cache_NewStack();
	Cache_Init(cache, this->storage, this->name);

	try (&exc) {
		call(Request, dl, cache);
	} clean finally {
		Downloader_Destroy(dl);
		Cache_Destroy(cache);
	} tryEnd;
}
