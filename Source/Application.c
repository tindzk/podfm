#import "Application.h"

#define self Application

def(void, Init, StorageInstance storage) {
	this->storage   = storage;
	this->providers = Providers_New(16);
}

def(void, Destroy) {
	foreach (provider, this->providers) {
		ProviderFacade_Destroy(*provider);
		ProviderFacade_Free(*provider);
	}

	Providers_Free(this->providers);
}

def(StorageInstance, GetStorage) {
	return this->storage;
}

def(ProviderFacadeInstance, AddProvider, String id) {
	size_t i;

	for (i = 0; i < ProviderListing_Length; i++) {
		if (String_Equals(ProviderListing[i]->id, id)) {
			goto found;
		}
	}

	when (found) {
		ProviderFacadeInstance provider = ProviderFacade_New();

		ProviderFacade_Init(provider, this->storage, ProviderListing[i]);

		Providers_Push(&this->providers, provider);

		return provider;
	}

	return ProviderFacade_Null();
}

def(void, Retrieve) {
	for (size_t i = 0; i < this->providers->len; i++) {
		ProviderFacadeInstance provider = this->providers->buf[i];

		Logger *logger = Debugger_GetLogger(Debugger_GetInstance());

		Logger_Info(logger, $("Processing provider '%'..."),
			ProviderFacade_GetName(provider));

		try {
			ProviderFacade_Retrieve(provider);
		} clean catch(Downloader, DownloadFailed) {
			Logger_Error(logger, $("Writing failed. Disk full?"));
			excBreak;
		} finally {

		} tryEnd;
	}
}
