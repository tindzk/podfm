#import "Provider.h"

def(void, Init, Logger *logger, Storage *storage, ProviderInterface *itf, String providerId) {
	this->logger          = logger;
	this->storage         = storage;
	this->backend.methods = itf;
	this->providerId      = String_Clone(providerId);

	Array_Init(this->sources, 10);

	this->backend.context = Memory_Alloc(itf->size);
	itf->init(this->backend.context, this->logger);

	Cache_Init(&this->cache, storage, logger, providerId);
}

def(void, Destroy) {
	this->backend.methods->destroy(this->backend.context);

	Memory_Free(this->backend.context);

	String_Destroy(&this->providerId);

	Array_Foreach(this->sources, String_Destroy);
	Array_Destroy(this->sources);

	Cache_Destroy(&this->cache);
}

def(void, AddSource, String source) {
	Array_Push(this->sources, String_Clone(source));
}

def(void, Retrieve) {
	Downloader dl;
	Downloader_Init(&dl, this->storage, this->logger, this->providerId);

	for (size_t i = 0; i < this->sources->len; i++) {
		String source = this->sources->buf[i];

		Logger_LogFmt(this->logger, Logger_Level_Info,
			$("Requesting listing for source '%'..."), source);

		Listing *listing;
		Array_Init(listing, 128);

		this->backend.methods->getListing(this->backend.context,
			source, listing);

		Logger_LogFmt(this->logger, Logger_Level_Info,
			$("% items found."),
			Integer_ToString(listing->len));

		for (size_t j = 0; j < listing->len; j++) {
			Logger_LogFmt(this->logger, Logger_Level_Info,
				$("[%/%] Fetching podcast '%' (%)..."),
				Integer_ToString(j + 1),
				Integer_ToString(listing->len),
				listing->buf[j].title,
				listing->buf[j].id);

			if (Cache_Has(&this->cache, listing->buf[j].id)) {
				Logger_Log(this->logger, Logger_Level_Info,
					$("Already in cache. Skipping..."));

				goto next;
			}

			this->backend.methods->fetch(this->backend.context,
				&dl, listing->buf[j]);

			Cache_Add(&this->cache, listing->buf[j].id);

		next:
			String_Destroy(&listing->buf[j].id);
			String_Destroy(&listing->buf[j].title);
			this->backend.methods->destroyItem(listing->buf[j].data);
			Memory_Free(listing->buf[j].data);
		}

		Array_Destroy(listing);
	}

	Downloader_Destroy(&dl);
}
