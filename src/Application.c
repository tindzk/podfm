#import "Application.h"
#import <App.h>

static ProviderInfo* providers[] = {
	/* French */
	&Providers_RFI_Info,

	/* General */
	&Providers_RSS_Info
};

def(void, Init, Logger *logger, StorageClass storage) {
	this->logger  = logger;
	this->storage = storage;

	Array_Init(this->providers, 16);
}

def(void, Destroy) {
	Array_Foreach(this->providers, ^(ProviderInstance *instance) {
		Provider_Destroy(instance->provider);
		Provider_Free(instance->provider);
	});

	Array_Destroy(this->providers);
}

def(StorageClass, GetStorage) {
	return this->storage;
}

def(ProviderClass, AddProvider, String id) {
	size_t i;

	for (i = 0; i < nElems(providers); i++) {
		if (String_Equals(providers[i]->id, id)) {
			goto found;
		}
	}

	when (found) {
		ProviderInstance instance;

		instance.item     = providers[i];
		instance.provider = Provider_New();

		Provider_Init(instance.provider,
			this->logger,
			this->storage,
			&providers[i]->methods,
			providers[i]->id);

		Array_Push(this->providers, instance);

		return instance.provider;
	}

	return Provider_Null();
}

def(void, Retrieve) {
	for (size_t i = 0; i < this->providers->len; i++) {
		ProviderClass members = this->providers->buf[i].provider;

		Logger_LogFmt(this->logger, Logger_Level_Info,
			$("Processing provider '%'..."),
			Provider_GetName(members));

		Provider_Retrieve(members);
	}
}
