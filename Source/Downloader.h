#import <URL.h>
#import <File.h>
#import <Float.h>
#import <String.h>
#import <Logger.h>
#import <FileStream.h>
#import <HTTP/Client.h>
#import <BufferedStream.h>
#import <Terminal/ProgressBar.h>

#import "Utils.h"
#import "Podcast.h"
#import "Storage.h"
#import "Debugger.h"

#define self Downloader

class {
	StorageInstance storage;
	Logger  *logger;
	String  providerId;
	bool    inclDate;
	String  location;
};

// @exc DownloadFailed

def(void, Init, StorageInstance storage, String providerId);
def(void, Destroy);
def(void, SetInclDate, bool value);
def(void, Get, String prefix, ListingItem *item, String url);
def(void, SaveText, String prefix, ListingItem *item, String text);

#undef self
