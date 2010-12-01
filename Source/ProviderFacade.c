#import "ProviderFacade.h"

def(void, Init, StorageInstance storage, ProviderInterface *provider) {
	this->logger   = Debugger_GetLogger(Debugger_GetInstance());
	this->storage  = storage;
	this->name     = String_Clone(provider->id);
	this->limit    = -1;
	this->sources  = StringArray_New(10);
	this->provider = provider;

	if (provider->size > 0) {
		this->instance = Generic_New(provider->size);
	} else {
		this->instance = Generic_Null();
	}

	if (this->provider->init != NULL) {
		this->provider->init(this->instance);
	}
}

def(void, Destroy) {
	if (this->provider->destroy != NULL) {
		this->provider->destroy(this->instance);
	}

	if (!Generic_IsNull(this->instance)) {
		Generic_Free(this->instance);
	}

	String_Destroy(&this->name);

	foreach (source, this->sources) {
		String_Destroy(source);
	}

	StringArray_Free(this->sources);
}

def(String, GetName) {
	return String_Disown(this->name);
}

def(void, SetName, String name) {
	String_Copy(&this->name, name);
}

def(void, SetLimit, s32 limit) {
	this->limit = limit;
}

def(bool, SetOption, String key, String value) {
	if (this->provider->setOption != NULL) {
		return this->provider->setOption(this->instance,
			key, value);
	}

	return false;
}

def(void, AddSource, String source) {
	Array_Push(this->sources, String_Clone(source));
}

static def(void, DestroyItem, ListingItem *item) {
	String_Destroy(&item->id);
	String_Destroy(&item->title);
	this->provider->destroyItem(item);
	Memory_Free(item);
}

static def(void, Fetch, DownloaderInstance dl, CacheInstance cache, Listing *listing) {
	for (size_t i = 0; i < listing->len; i++) {
		if (this->limit >= 0 && i >= (size_t) this->limit) {
			call(DestroyItem, listing->buf[i]);
			continue;
		}

		if (listing->buf[i]->id.len == 0) {
			Logger_Error(this->logger,
				$("[%/%] The ID is empty!"),
				Int32_ToString(i + 1),
				Int32_ToString(listing->len));

			call(DestroyItem, listing->buf[i]);
			continue;
		}

		Logger_Info(this->logger,
			$("[%/%] Fetching podcast '%' (%)..."),
			Int32_ToString(i + 1),
			Int32_ToString(listing->len),
			listing->buf[i]->title,
			listing->buf[i]->id);

		if (Cache_Has(cache, listing->buf[i]->id)) {
			Logger_Info(this->logger,
				$("Already in cache. Skipping..."));

			call(DestroyItem, listing->buf[i]);
			continue;
		}

		try {
			this->provider->fetch(this->instance, dl,
				listing->buf[i]);

			Cache_Add(cache, listing->buf[i]->id);
		} clean catchAny {
			for (size_t j = i + 1; j < listing->len; j++) {
				call(DestroyItem, listing->buf[j]);
			}

			__exc_rethrow = true;
		} finally {
			call(DestroyItem, listing->buf[i]);
		} tryEnd;
	}
}

static def(void, Request, DownloaderInstance dl, CacheInstance cache) {
	for (size_t i = 0; i < this->sources->len; i++) {
		String source = this->sources->buf[i];

		Logger_Info(this->logger,
			$("Requesting listing for source '%'..."), source);

		Listing *listing = Listing_New(128);

		try {
			this->provider->getListing(this->instance, source, &listing);

			Logger_Info(this->logger, $("% items found (limit=%)."),
				Int32_ToString(listing->len),
				(this->limit == -1)
					? $("no")
					: Int32_ToString(this->limit));

			call(Fetch, dl, cache, listing);
		} clean finally {
			Listing_Free(listing);
		} tryEnd;
	}
}

def(void, Retrieve) {
	Downloader dl;
	Downloader_Init(&dl, this->storage, this->name);

	Cache cache;
	Cache_Init(&cache, this->storage, this->name);

	try {
		call(Request, &dl, &cache);
	} clean finally {
		Downloader_Destroy(&dl);
		Cache_Destroy(&cache);
	} tryEnd;
}