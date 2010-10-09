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

#import "Utils.h"
#import "Podcast.h"
#import "Storage.h"

#undef self
#define self Downloader

typedef struct {
	Storage *storage;
	Logger  *logger;
	String  providerId;
	bool    inclDate;
} Downloader;

def(void, Init, Storage *storage, Logger *logger, String providerId);
def(void, Destroy);
def(void, SetInclDate, bool value);
def(void, Get, Podcast podcast, String url);
def(void, SaveText, Podcast podcast, String text);
