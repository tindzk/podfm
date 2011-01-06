#import "Providers.h"

ExportAnonImplEx(Providers_RSS, Provider);
ExportAnonImplEx(Providers_RFI, Provider);

const ProviderInterface* ProviderListing[] = {
	/* French */
	&Providers_RFI_ProviderImpl,

	/* General */
	&Providers_RSS_ProviderImpl
};

const size_t ProviderListing_Length = nElems(ProviderListing);
