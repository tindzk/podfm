#import <Logger.h>
#import <String.h>
#import <Terminal.h>

#define $(s) String(s)
#define when(lbl) if (0) lbl: if (1)

void Utils_OnLogMessage(Logger *this, String msg, Logger_Level level, String file, int line);
