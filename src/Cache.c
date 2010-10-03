#import "Cache.h"

size_t Modules_Cache;

static ExceptionManager *exc;

void Cache0(ExceptionManager *e) {
	Modules_Cache = Module_Register(String("Cache"));

	exc = e;
}

def(void, Init, Storage *storage, Logger *logger, String provider) {
	String path = Storage_GetCachePath(storage, provider);

	File_Open(&this->file, path,
		FileStatus_ReadWrite |
		FileStatus_Append    |
		FileStatus_Create);

	String_Destroy(&path);

	String s = HeapString(100 * 1024);

	s.len = File_Read(&this->file, s.buf, s.size);

	if (s.len == s.size) {
		Logger_Log(logger, Logger_Level_Fatal,
			$("The index file is too large. Remove all unneeded lines."));

		throw(exc, excIndexFileTooLarge);
	}

	this->items = String_Split(s, '\n');

	for (size_t i = 0; i < this->items->len; i++) {
		this->items->buf[i] = String_Clone(this->items->buf[i]);
	}

	String_Destroy(&s);
}

def(void, Destroy) {
	File_Close(&this->file);

	Array_Foreach(this->items, String_Destroy);
	Array_Destroy(this->items);
}

def(bool, Has, String id) {
	for (size_t i = 0; i < this->items->len; i++) {
		if (String_Equals(this->items->buf[i], id)) {
			return true;
		}
	}

	return false;
}

def(void, Add, String id) {
	String line = String_Format($("%\n"), id);
	File_Write(&this->file, line.buf, line.len);
	String_Destroy(&line);

	Array_Push(this->items, String_Clone(id));
}
