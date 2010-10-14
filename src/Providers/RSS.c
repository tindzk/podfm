#import "RSS.h"
#import <App.h>

typedef struct {
	Logger *logger;
} Class(self);

extern ExceptionManager exc;

def(void, Init, Logger *logger) {
	this->logger = logger;
}

def(void, Destroy) {

}

void ref(DestroyItem)(DefaultPodcastItem *item) {
	String_Destroy(&item->podcast);
}

def(void, GetListing, String url, Listing *res) {
	URL_Parts parts = URL_Parse(url);

	HTTP_Client client;
	HTTP_Client_Init(&client);
	HTTP_Client_Open(&client, parts.host);

	try (&exc) {
		HTTP_Client_Request(&client, parts.path);
		HTTP_Client_FetchResponse(&client);
	} finally {
		URL_Parts_Destroy(&parts);
	} tryEnd;

	String buf = HTTP_Client_Read(&client, 50 * 1024);

	HTTP_Client_Destroy(&client);

	String s;
	ssize_t pos = 0;

	while ((pos = String_Between(buf, pos, $("<item>"), $("</item>"), &s)) != String_NotFound) {
		Podcast item;

		/* Title */
		item.title =
			String_Clone(
				String_Trim(
					String_Between(s,
						$("<title>"),
						$("</title>"))));

		/* ID */
		item.id = String_Between(s,
			$("<guid"),
			$("</guid>"));

		ssize_t del = String_Find(item.id, $(">"));

		if (del != String_NotFound) {
			item.id = String_Slice(item.id, del + 1);
		}

		item.id = String_Clone(item.id);

		/* Date */
		Date_RFC822 date =
			Date_RFC822_Parse(
				String_Between(s,
					$("<pubDate>"),
					$("</pubDate>")));

		item.date = ((DateTime *) &date)->date;

		/* URL */
		DefaultPodcastItem *data = New(DefaultPodcastItem);
		data->podcast = String_Clone(String_Between(s, $("<enclosure url=\""), $("\"")));
		item.data = data;

		Array_Push(res, item);
	}

	String_Destroy(&buf);
}

def(void, Fetch, DownloaderClass dl, Podcast item) {
	DefaultPodcastItem *data = item.data;

	Downloader_Get(dl, item, data->podcast);
}

ProviderInfo ref(Info) = {
	.id      = $("rss"),
	.methods = {
		.size        = sizeof(self),
		.init        = (void *) ref(Init),
		.destroy     = (void *) ref(Destroy),
		.destroyItem = (void *) ref(DestroyItem),
		.getListing  = (void *) ref(GetListing),
		.fetch       = (void *) ref(Fetch)
	}
};