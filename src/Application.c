#import "Application.h"
#import <App.h>

extern ExceptionManager exc;

static ProviderInterface* providers[] = {
	/* French */
	&Providers_RFI_ProviderImpl,

	/* General */
	&Providers_RSS_ProviderImpl
};

def(void, Init, StorageInstance storage) {
	this->storage   = storage;
	this->providers = Providers_New(16);
}

def(void, Destroy) {
	Array_Foreach(this->providers, ^(ProviderFacadeInstance *provider) {
		ProviderFacade_Destroy(*provider);
		ProviderFacade_Free(*provider);
	});

	Providers_Free(this->providers);
}

def(StorageInstance, GetStorage) {
	return this->storage;
}

def(ProviderFacadeInstance, AddProvider, String id) {
	size_t i;

	for (i = 0; i < nElems(providers); i++) {
		if (String_Equals(providers[i]->id, id)) {
			goto found;
		}
	}

	when (found) {
		ProviderFacadeInstance provider = ProviderFacade_New();

		ProviderFacade_Init(provider, this->storage, providers[i]);

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

		try (&exc) {
			ProviderFacade_Retrieve(provider);
		} clean catch(Downloader, excDownloadFailed) {
			Logger_Error(logger, $("Writing failed. Disk full?"));
			excBreak;
		} finally {

		} tryEnd;
	}
}
