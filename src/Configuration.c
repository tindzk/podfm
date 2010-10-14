#import "Configuration.h"

#import <App.h>

def(void, Init, ApplicationClass app, Logger *logger) {
	this->app    = app;
	this->logger = logger;
}

static def(ProviderClass, NewProvider, String name) {
	ProviderClass provider = Application_AddProvider(this->app,
		name);

	if (Provider_IsNull(provider)) {
		Logger_LogFmt(this->logger, Logger_Level_Error,
			$("Plugin '%' not found!"),
			name);
	}

	return provider;
}

static def(void, ParseSubscriptions, YAML_Node *node) {
	for (size_t i = 0; i < node->len; i++) {
		YAML_Node *child = node->nodes[i];
		ProviderClass provider = Provider_Null();

		if (child->type == YAML_NodeType_Item) {
			Logger_LogFmt(this->logger, Logger_Level_Debug,
				$("Adding provider '%' (%)..."),
				YAML_Item(child)->value,
				YAML_Item(child)->key);

			provider = call(NewProvider, YAML_Item(child)->key);

			if (!Provider_IsNull(provider)) {
				Provider_SetName(provider,
					YAML_Item(child)->value);
			}
		} else if (child->type == YAML_NodeType_Section) {
			Logger_LogFmt(this->logger, Logger_Level_Debug,
				$("Adding provider '%'..."),
				YAML_Section(child)->name);

			provider = call(NewProvider, YAML_Section(child)->name);
		}

		if (!Provider_IsNull(provider) && child->len > 0) {
			for (size_t j = 0; j < child->nodes[0]->len; j++) {
				YAML_Node *_child = child->nodes[0]->nodes[j];

				if (_child->type == YAML_NodeType_Item) {
					String key   = YAML_Item(_child)->key;
					String value = YAML_Item(_child)->value;

					if (String_Equals(key, $("limit"))) {
						Provider_SetLimit(provider,
							Integer_ParseString(value));
					} else if (String_Equals(key, $("date"))) {
						Provider_SetInclDate(provider,
							String_Equals(value, $("yes")));
					} else {
						Provider_AddSource(provider,
							YAML_Item(_child)->value);
					}
				}
			}
		}
	}
}

def(void, Parse) {
	YAML yml;
	File file;
	BufferedStream stream;

	StorageClass storage = Application_GetStorage(this->app);

	String path = Storage_GetCfgPath(storage);

	if (!Path_Exists(path)) {
		Logger_LogFmt(this->logger, Logger_Level_Fatal,
			$("The subscriptions file % does not exist!"),
			path);

		String_Destroy(&path);

		return;
	}

	FileStream_Open(&file, path, FileStatus_ReadOnly);

	String_Destroy(&path);

	BufferedStream_Init(&stream, &FileStream_Methods, &file);
	BufferedStream_SetInputBuffer(&stream, 1024, 128);

	YAML_Init(&yml, 4, &BufferedStream_Methods, &stream);
	YAML_Parse(&yml);

	call(ParseSubscriptions, YAML_GetRoot(&yml));

	YAML_Destroy(&yml);

	BufferedStream_Close(&stream);
	BufferedStream_Destroy(&stream);
}
