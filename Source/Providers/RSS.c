#import <URL.h>
#import <Date.h>
#import <HTTP/Client.h>
#import <Date/RFC822.h>
#import <HTML/Entities.h>

#import "../ProviderInterface.h"

#undef self
#define self Providers_RSS

class {
	bool inclDate;
};

def(void, Init) {
	this->inclDate = true;
}

def(bool, SetOption, String key, String value) {
	if (String_Equals(key, $("date"))) {
		this->inclDate = String_Equals(value, $("yes"));
		return true;
	}

	return false;
}

sdef(void, DestroyItem, DefaultPodcastItem *item) {
	String_Destroy(&item->podcast);
}

def(void, GetListing, String url, Listing **res) {
	URL_Parts parts = URL_Parse(url);

	HTTP_Client client;
	HTTP_Client_Init(&client);

	String buf;

	try {
		HTTP_Client_Open(&client, parts.host);
		HTTP_Client_Request(&client, parts.path);
		HTTP_Client_FetchResponse(&client);
		buf = HTTP_Client_Read(&client, 50 * 1024);
	} clean finally {
		URL_Parts_Destroy(&parts);
		HTTP_Client_Destroy(&client);
	} tryEnd;

	String s;
	ssize_t pos = 0;

	while ((pos = String_Between(buf, pos, $("<item>"), $("</item>"), &s)) != String_NotFound) {
		/* Date */
		Date_RFC822 rfcDate =
			Date_RFC822_Parse(
				String_Between(s,
					$("<pubDate>"),
					$("</pubDate>")));

		/* ID */
		String id = String_Between(s,
			$("<guid"),
			$("</guid>"));

		ssize_t del = String_Find(id, $(">"));

		if (del != String_NotFound) {
			id = String_Slice(id, del + 1);
		}

		/* Title */
		String title =
			HTML_Entities_Decode(
				String_Between(s,
					$("<title>"),
					$("</title>")));

		String_Trim(&title);

		DefaultPodcastItem *item = New(DefaultPodcastItem);

		item->base = (ListingItem) {
			.id    = String_Clone(id),
			.title = title
		};

		item->date    = rfcDate.date;
		item->podcast = String_Clone(
				String_Between(s,
					$("<enclosure url=\""),
					$("\"")));

		Listing_Push(res, (ListingItem *) item);
	}

	String_Destroy(&buf);
}

def(void, Fetch, DownloaderInstance dl, ListingItem *item) {
	DefaultPodcastItem *data = (DefaultPodcastItem *) item;

	String prefix = HeapString(0);

	if (this->inclDate) {
		prefix = Utils_GetDate(data->date);
	}

	try {
		Downloader_Get(dl, prefix, item, data->podcast);
	} clean finally {
		String_Destroy(&prefix);
	} tryEnd;
}

ImplEx(Provider) = {
	.id          = $("rss"),
	.size        = sizeof(self),
	.init        = (void *) ref(Init),
	.destroy     = NULL,
	.destroyItem = (void *) ref(DestroyItem),
	.setOption   = (void *) ref(SetOption),
	.getListing  = (void *) ref(GetListing),
	.fetch       = (void *) ref(Fetch)
};
