#import "Application.h"

static ProviderInfo* providers[] = {
	/* French */
	&Providers_RFI_Info,

	/* General */
	&Providers_RSS_Info
};

def(void, Init, Logger *logger, Storage *storage) {
	this->logger  = logger;
	this->storage = storage;

	Array_Init(this->providers, 16);
}

def(void, Destroy) {
	Array_Foreach(this->providers, ^(ProviderInstance *instance) {
		Provider_Destroy(instance->provider);
		Memory_Free(instance->provider);
	});

	Array_Destroy(this->providers);
}

def(Storage *, GetStorage) {
	return this->storage;
}

def(Provider *, AddProvider, String id) {
	size_t i;

	for (i = 0; i < sizeof(providers) / sizeof(providers[0]); i++) {
		if (String_Equals(providers[i]->id, id)) {
			goto found;
		}
	}

	when (found) {
		ProviderInstance instance;

		instance.item     = providers[i];
		instance.provider = New(Provider);

		Provider_Init(instance.provider,
			this->logger,
			this->storage,
			&providers[i]->methods,
			providers[i]->id);

		Array_Push(this->providers, instance);

		return instance.provider;
	}

	return NULL;
}

def(void, Retrieve) {
	for (size_t i = 0; i < this->providers->len; i++) {
		ProviderInstance instance = this->providers->buf[i];

		Logger_LogFmt(this->logger, Logger_Level_Info,
			$("Processing provider '%'..."),
			Provider_GetName(instance.provider));

		Provider_Retrieve(instance.provider);
	}
}
