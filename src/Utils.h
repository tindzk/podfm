#import <Logger.h>
#import <String.h>
#import <Terminal.h>

#define $(s) String(s)

void Utils_OnLogMessage(Logger *this, String msg, Logger_Level level, String file, int line);
