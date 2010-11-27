#import <Path.h>
#import <File.h>
#import <YAML.h>
#import <Socket.h>
#import <Signal.h>
#import <Terminal.h>
#import <Exception.h>
#import <FileStream.h>
#import <HTTP/Client.h>
#import <BufferedStream.h>

#import "Cache.h"
#import "Storage.h"
#import "Debugger.h"
#import "Application.h"
#import "Configuration.h"

ExceptionManager exc;

Terminal term;

int main(int argc, char *argv[]) {
	Signal0();

	Terminal_Init(&term, File_StdIn, File_StdOut, true);
	Terminal_Configure(&term, false, true);

	String path = (argc > 1)
		? String_FromNul(argv[1])
		: $(DefaultPath);

	Storage storage;
	Storage_Init(&storage, path);

	Application app;
	Application_Init(&app, &storage);

	try {
		Configuration cfg;
		Configuration_Init(&cfg, &app);
		Configuration_Parse(&cfg);

		Application_Retrieve(&app);
	} clean catchAny {
		Exception_Print(e);

#if Exception_SaveTrace
		Backtrace_PrintTrace(exc.e.trace, exc.e.traceItems);
#endif

		excReturn ExitStatus_Failure;
	} finally {
		Application_Destroy(&app);
		Storage_Destroy(&storage);

		Terminal_Destroy(&term);
	} tryEnd;

	return ExitStatus_Success;
}
