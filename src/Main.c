#import <Path.h>
#import <File.h>
#import <YAML.h>
#import <Socket.h>
#import <Signal.h>
#import <Logger.h>
#import <Terminal.h>
#import <FileStream.h>
#import <HTTP/Client.h>
#import <BufferedStream.h>
#import <ExceptionManager.h>

#import "Utils.h"
#import "Cache.h"
#import "Storage.h"
#import "Application.h"
#import "Configuration.h"

ExceptionManager exc;

Terminal term;

int main(int argc, char *argv[]) {
	ExceptionManager_Init(&exc);

	Path0(&exc);
	YAML0(&exc);
	File0(&exc);
	Signal0(&exc);
	Memory0(&exc);
	Socket0(&exc);
	String0(&exc);
	Integer0(&exc);
	HTTP_Header0(&exc);
	HTTP_Client0(&exc);
	NetworkAddress0(&exc);
	SocketConnection0(&exc);

	Cache0(&exc);

	Logger logger;

	Logger_Init(&logger, (void *) &Utils_OnLogMessage, &logger,
		Logger_Level_Fatal |
		Logger_Level_Crit  |
		Logger_Level_Error |
		Logger_Level_Warn  |
		Logger_Level_Info  |
		Logger_Level_Debug |
		Logger_Level_Trace);

	int res = EXIT_SUCCESS;

	Terminal_Init(&term, File_StdIn, File_StdOut, true);
	Terminal_Configure(&term, false, true);

	String path = (argc > 1)
		? String_FromNul(argv[1])
		: $(DefaultPath);

	Storage storage;
	Storage_Init(&storage, path);

	Application app;
	Application_Init(&app, &logger, &storage);

	try (&exc) {
		Configuration cfg;
		Configuration_Init(&cfg, &app, &logger);
		Configuration_Parse(&cfg);

		Application_Retrieve(&app);
	} catchAny(e) {
		Exception_Print(e);

#if Exception_SaveTrace
		Backtrace_PrintTrace(e->trace, e->traceItems);
#endif

		res = EXIT_FAILURE;
	} finally {
		Application_Destroy(&app);
		Storage_Destroy(&storage);

		Terminal_Destroy(&term);
	} tryEnd;

	return res;
}
