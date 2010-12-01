#import "Utils.h"

#define self Utils

sdef(String, GetDate, Date date) {
	String year  = Number_Format(date.year,  4);
	String month = Number_Format(date.month, 2);
	String day   = Number_Format(date.day,   2);

	String res = String_Format($("%-%-%"), year, month, day);

	String_Destroy(&day);
	String_Destroy(&month);
	String_Destroy(&year);

	return res;
}
