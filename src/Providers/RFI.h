#import <URL.h>
#import <Date.h>
#import <String.h>
#import <Logger.h>
#import <HTTP/Client.h>
#import <Date/RFC822.h>
#import <HTML/Entities.h>

#import "../Utils.h"
#import "../Provider.h"

#undef self
#define self Providers_RFI

typedef struct {
	Logger *logger;
	HTTP_Client client;
} self;

ProviderInfo ref(Info);
