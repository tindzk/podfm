#import "Cache.h"

#define self Cache

def(void, Init, StorageInstance storage, String provider) {
	String path = Storage_GetCachePath(storage, provider);

	File_Open(&this->file, path,
		FileStatus_ReadWrite |
		FileStatus_Append    |
		FileStatus_Create);

	String_Destroy(&path);

	String s = String_New(100 * 1024);

	File_Read(&this->file, &s);

	if (s.len == String_GetSize(&s)) {
		Logger *logger = Debugger_GetLogger(Debugger_GetInstance());

		Logger_Fatal(logger,
			$("The index file is too large. Remove all unneeded lines."));

		throw(IndexFileTooLarge);
	}

	this->items = String_Split(&s, '\n');

	String_Destroy(&s);
}

def(void, Destroy) {
	File_Close(&this->file);

	StringArray_Destroy(this->items);
	StringArray_Free(this->items);
}

def(bool, Has, String id) {
	return StringArray_Contains(this->items, id);
}

def(void, Add, String id) {
	String line = String_Concat(id, '\n');
	File_Write(&this->file, line);
	String_Destroy(&line);

	String *strId = New(String);
	*strId = String_Clone(id);

	StringArray_Push(&this->items, strId);
}
