#import <URL.h>
#import <Date.h>
#import <HTTP/Client.h>
#import <Date/RFC822.h>
#import <HTML/Entities.h>

#import "../ProviderInterface.h"

#import <App.h>

#undef self
#define self Providers_RSS

class(self) {

};

extern ExceptionManager exc;

void ref(DestroyItem)(DefaultPodcastItem *item) {
	String_Destroy(&item->podcast);
}

def(void, GetListing, String url, Listing *res) {
	URL_Parts parts = URL_Parse(url);

	HTTP_Client client;
	HTTP_Client_Init(&client);

	String buf;

	try (&exc) {
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
		Podcast item;

		/* Title */
		item.title =
			HTML_Entities_Decode(
				String_Between(s,
					$("<title>"),
					$("</title>")));

		String_Trim(&item.title);

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

def(void, Fetch, DownloaderInstance dl, Podcast item) {
	DefaultPodcastItem *data = item.data;

	Downloader_Get(dl, item, data->podcast);
}

ProviderInterface ref(ProviderImpl) = {
	.id          = $("rss"),
	.size        = sizeof(self),
	.init        = NULL,
	.destroy     = NULL,
	.destroyItem = (void *) ref(DestroyItem),
	.getListing  = (void *) ref(GetListing),
	.fetch       = (void *) ref(Fetch)
};
