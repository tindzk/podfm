#import "Configuration.h"

#define self Configuration

def(void, Init, ApplicationInstance app) {
	this->app    = app;
	this->logger = Debugger_GetLogger(Debugger_GetInstance());
}

static def(ProviderFacadeInstance, NewProvider, String name) {
	ProviderFacadeInstance provider =
		Application_AddProvider(this->app, name);

	if (ProviderFacade_IsNull(provider)) {
		Logger_Error(this->logger, $("Plugin '%' not found!"),
			name);
	}

	return provider;
}

static def(void, ParseSubscriptions, YAML_Node *node) {
	for (size_t i = 0; i < node->len; i++) {
		YAML_Node *child = node->buf[i];
		ProviderFacadeInstance provider = ProviderFacade_Null();

		if (child->type == YAML_NodeType_Item) {
			Logger_Debug(this->logger,
				$("Adding provider '%' (%)..."),
				YAML_Item(child)->value,
				YAML_Item(child)->key);

			provider = call(NewProvider, YAML_Item(child)->key);

			if (!ProviderFacade_IsNull(provider)) {
				ProviderFacade_SetName(provider,
					YAML_Item(child)->value);
			}
		} else if (child->type == YAML_NodeType_Section) {
			Logger_Debug(this->logger,
				$("Adding provider '%'..."),
				YAML_Section(child)->name);

			provider = call(NewProvider, YAML_Section(child)->name);
		}

		if (!ProviderFacade_IsNull(provider) && child->len > 0) {
			for (size_t j = 0; j < child->buf[0]->len; j++) {
				YAML_Node *_child = child->buf[0]->buf[j];

				if (_child->type == YAML_NodeType_Item) {
					String key   = YAML_Item(_child)->key;
					String value = YAML_Item(_child)->value;

					if (String_Equals(key, $("limit"))) {
						ProviderFacade_SetLimit(provider,
							Int32_Parse(value));
					} else {
						bool handled = ProviderFacade_SetOption(
							provider, key, value);

						if (!handled) {
							ProviderFacade_AddSource(provider,
								YAML_Item(_child)->value);
						}
					}
				}
			}
		}
	}
}

def(void, Parse) {
	String path =
		Storage_GetCfgPath(
			Application_GetStorage(this->app));

	if (!Path_Exists(path)) {
		Logger_Fatal(this->logger,
			$("The subscriptions file % does not exist!"), path);

		goto out;
	}

	File file;
	File_Open(&file, path, FileStatus_ReadOnly);

	BufferedStream stream;
	BufferedStream_Init(&stream, File_AsStream(&file));
	BufferedStream_SetInputBuffer(&stream, 1024, 128);

	YAML yml;
	YAML_Init(&yml, 4, &BufferedStreamImpl, &stream);
	YAML_Parse(&yml);

	call(ParseSubscriptions, YAML_GetRoot(&yml));

	YAML_Destroy(&yml);

	BufferedStream_Close(&stream);
	BufferedStream_Destroy(&stream);

out:
	String_Destroy(&path);
}
