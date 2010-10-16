#import "Application.h"
#import <App.h>

static ProviderInterface* providers[] = {
	/* French */
	&Providers_RFI_ProviderImpl,

	/* General */
	&Providers_RSS_ProviderImpl
};

def(void, Init, StorageInstance storage) {
	this->logger  = Debugger_GetLogger(Debugger_GetInstance());
	this->storage = storage;

	Array_Init(this->providers, 16);
}

def(void, Destroy) {
	Array_Foreach(this->providers, ^(ProviderFacadeInstance *provider) {
		ProviderFacade_Destroy(*provider);
		ProviderFacade_Free(*provider);
	});

	Array_Destroy(this->providers);
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

		Array_Push(this->providers, provider);

		return provider;
	}

	return ProviderFacade_Null();
}

def(void, Retrieve) {
	for (size_t i = 0; i < this->providers->len; i++) {
		ProviderFacadeInstance provider = this->providers->buf[i];

		Logger_LogFmt(this->logger, Logger_Level_Info,
			$("Processing provider '%'..."),
			ProviderFacade_GetName(provider));

		ProviderFacade_Retrieve(provider);
	}
}
