#import <Path.h>
#import <String.h>

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

#undef self
