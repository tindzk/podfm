#import "Storage.h"

def(void, Init, String base) {
	this->base = String_Clone(base);
}

def(void, Destroy) {
	String_Destroy(&this->base);
}

def(String, GetBase) {
	return this->base;
}

def(String, BuildPath, String provider) {
	String res = String_Format($("%/%"), this->base, provider);

	if (!Path_Exists(res)) {
		Path_Create(res);
	}

	return res;
}

def(String, GetCachePath, String provider) {
	return String_Format($("%/%.cache"), this->base, provider);
}

def(String, GetCfgPath) {
	return String_Format($("%/subscriptions.yml"), this->base);
}
