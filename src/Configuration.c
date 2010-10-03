#import "Configuration.h"

def(void, Init, Application *app, Logger *logger) {
	this->app    = app;
	this->logger = logger;
}

static def(void, ParseSubscriptions, YAML_Node *node) {
	for (size_t i = 0; i < node->len; i++) {
		YAML_Node *child = node->nodes[i];

		if (child->type == YAML_NodeType_Section) {
			Logger_LogFmt(this->logger, Logger_Level_Debug,
				$("Adding provider '%'..."),
				YAML_Section(child)->name);

			Provider *provider = Application_AddProvider(this->app,
				YAML_Section(child)->name);

			if (provider == NULL) {
				Logger_LogFmt(this->logger, Logger_Level_Error,
					$("Provider '%' not found!"),
					YAML_Section(child)->name);

				return;
			}

			if (child->len > 0) {
				for (size_t j = 0; j < child->nodes[0]->len; j++) {
					YAML_Node *_child = child->nodes[0]->nodes[j];

					if (_child->type == YAML_NodeType_Item) {
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

	Storage *storage = Application_GetStorage(this->app);

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

	ref(ParseSubscriptions)(this, YAML_GetRoot(&yml));

	YAML_Destroy(&yml);

	BufferedStream_Close(&stream);
	BufferedStream_Destroy(&stream);
}
