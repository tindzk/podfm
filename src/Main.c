#import <Path.h>
#import <File.h>
#import <YAML.h>
#import <Socket.h>
#import <Signal.h>
#import <Terminal.h>
#import <FileStream.h>
#import <HTTP/Client.h>
#import <BufferedStream.h>
#import <ExceptionManager.h>

#import "Cache.h"
#import "Storage.h"
#import "Debugger.h"
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

	Terminal_Init(&term, File_StdIn, File_StdOut, true);
	Terminal_Configure(&term, false, true);

	String path = (argc > 1)
		? String_FromNul(argv[1])
		: $(DefaultPath);

	StorageInstance storage = Storage_NewStack();
	Storage_Init(storage, path);

	ApplicationInstance app = Application_NewStack();
	Application_Init(app, storage);

	try (&exc) {
		ConfigurationInstance cfg = Configuration_NewStack();
		Configuration_Init(cfg, app);
		Configuration_Parse(cfg);

		Application_Retrieve(app);
	} clean catchAny(e) {
		Exception_Print(e);

#if Exception_SaveTrace
		Backtrace_PrintTrace(e->trace, e->traceItems);
#endif

		excReturn ExitStatus_Failure;
	} finally {
		Application_Destroy(app);
		Storage_Destroy(storage);

		Terminal_Destroy(&term);
	} tryEnd;

	return ExitStatus_Success;
}
