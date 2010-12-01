#import <Path.h>
#import <String.h>

#undef self
#define self Storage

class {
	String base;
};

def(void, Init, String base);
def(void, Destroy);
def(String, GetBase);
def(String, BuildPath, String provider);
def(String, GetCachePath, String provider);
def(String, GetCfgPath);
