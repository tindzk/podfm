#import <URL.h>
#import <File.h>
#import <Float.h>
#import <String.h>
#import <Number.h>
#import <Logger.h>
#import <FileStream.h>
#import <HTTP/Client.h>
#import <BufferedStream.h>
#import <Terminal/ProgressBar.h>

#import "Podcast.h"
#import "Storage.h"
#import "Debugger.h"

#undef self
#define self Downloader

class(self) {
	StorageInstance storage;
	Logger  *logger;
	String  providerId;
	bool    inclDate;
	String  location;
};

ExtendClass(self);

def(void, Init, StorageInstance storage, String providerId);
def(void, Destroy);
def(void, SetInclDate, bool value);
def(void, Get, Podcast podcast, String url);
def(void, SaveText, Podcast podcast, String text);
