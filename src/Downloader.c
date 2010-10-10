#import "Downloader.h"

extern Terminal term;

def(void, Init, Storage *storage, Logger *logger, String providerId) {
	this->storage    = storage;
	this->logger     = logger;
	this->providerId = String_Clone(providerId);
	this->inclDate   = true;
	this->location   = HeapString(0);
}

def(void, Destroy) {
	String_Destroy(&this->providerId);
	String_Destroy(&this->location);
}

def(void, SetInclDate, bool value) {
	this->inclDate = value;
}

/* Try to auto-detect the extension. Fall back to MP3. */
static String ref(GetMediaExtension)(String path) {
	ssize_t pos = String_ReverseFind(path, '.');

	if (pos != String_NotFound) {
		return String_Slice(path, pos + 1);
	} else {
		return $("mp3");
	}
}

static String ref(Sanitize)(String name) {
	String out = String_Clone(name);

	String_ReplaceAll(&out, $("?"),  $(" "));
	String_ReplaceAll(&out, $(":"),  $(" "));
	String_ReplaceAll(&out, $("/"),  $("-"));
	String_ReplaceAll(&out, $("  "), $(" "));

	String_Trim(&out);

	if (out.len > 0 && out.buf[out.len - 1] == '.') {
		out.len--;
	}

	return out;
}

static String ref(GetDate)(Podcast podcast) {
	String year  = Number_Format(podcast.date.year,  4);
	String month = Number_Format(podcast.date.month, 2);
	String day   = Number_Format(podcast.date.day,   2);

	String res = String_Format($("%-%-% "), year, month, day);

	String_Destroy(&day);
	String_Destroy(&month);
	String_Destroy(&year);

	return res;
}

static def(String, BuildPath, Podcast podcast, String ext) {
	String title = ref(Sanitize)(podcast.title);
	String date  = $("");
	String path  = Storage_BuildPath(this->storage, this->providerId);

	if (this->inclDate) {
		date = ref(GetDate)(podcast);
	}

	String res = String_Format($("%/%%.%"),
		path, date, title, ext);

	String_Destroy(&path);
	String_Destroy(&date);
	String_Destroy(&title);

	return res;
}

static def(void, OnHeader, String name, String value) {
	if (String_Equals(name, $("Location"))) {
		String_Copy(&this->location, value);
	}
}

def(void, Get, Podcast podcast, String url) {
	this->location.len = 0;

	HTTP_Client client;

	URL_Parts parts = URL_Parse(url);

	HTTP_Client_Init(&client, parts.host);

	HTTP_Client_Events events;
	events.onVersion = NULL;
	events.onHeader  = (void *) &ref(OnHeader);
	events.context   = this;

	HTTP_Client_SetEvents(&client, events);

	HTTP_Client_Request(&client, parts.path);

	Logger_LogFmt(this->logger, Logger_Level_Debug,
		$("URL: %"), url);

	URL_Parts_Destroy(&parts);

	HTTP_Client_FetchResponse(&client);

	size_t cnt = 0;

	if (this->location.len > 0) {
		if (cnt > 3) {
			Logger_Log(this->logger, Logger_Level_Error,
				$("Redirection loop."));

			goto out;
		}

		cnt++;

		Logger_LogFmt(this->logger, Logger_Level_Debug,
			$("Redirecting..."),
			this->location);

		ref(Get)(this, podcast, this->location);

		goto out;
	}

	String full = ref(BuildPath)(this,
		podcast,
		ref(GetMediaExtension)(parts.path));

	Logger_LogFmt(this->logger, Logger_Level_Debug,
		$("Destination: %"), full);

	File file;
	File_Open(&file, full,
		FileStatus_Create   |
		FileStatus_Truncate |
		FileStatus_WriteOnly);

	String_Destroy(&full);

	BufferedStream output;
	BufferedStream_Init(&output, &FileStream_Methods, &file);
	BufferedStream_SetOutputBuffer(&output, 128 * 1024);

	uint64_t got  = 0;
	int64_t total = HTTP_Client_GetLength(&client);
	size_t prevPercent = 100;

	String buf = HeapString(HTTP_Client_ReadChunkSize);

	Terminal_ProgressBar pbar;
	Terminal_ProgressBar_Init(&pbar, &term);

	while (HTTP_Client_Read(&client, &buf)) {
		got += buf.len;
		BufferedStream_Write(&output, buf.buf, buf.len);

		size_t percent = (size_t) ((float) got / (float) total * 100);

		if (prevPercent != percent) {
			float gotmb   = (float) got   / 1024 / 1024;
			float totalmb = (float) total / 1024 / 1024;

			String msg = String_Format(
				$("Received % MiB of % MiB"),
				Float_ToString(gotmb,   0.01),
				Float_ToString(totalmb, 0.01));

			Terminal_ProgressBar_Render(&pbar, percent, msg);

			String_Destroy(&msg);
		}

		prevPercent = percent;
	}

	Terminal_ProgressBar_Clear(&pbar);

	String_Destroy(&buf);

	BufferedStream_Close(&output);
	BufferedStream_Destroy(&output);

out:
	HTTP_Client_Destroy(&client);
}

def(void, SaveText, Podcast podcast, String text) {
	File file;

	String path = ref(BuildPath)(this, podcast, $("txt"));

	File_Open(&file, path,
			FileStatus_Create   |
			FileStatus_Truncate |
			FileStatus_WriteOnly);

	File_Write(&file, text);

	File_Close(&file);

	Logger_LogFmt(this->logger, Logger_Level_Info,
		$("Saved transcript."),
		path, Integer_ToString(text.len));

	String_Destroy(&path);
}
