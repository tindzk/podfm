#import "RFI.h"
#import <App.h>

typedef struct {
	HTTP_Client client;
} Class(self);

extern ExceptionManager exc;

def(void, Init) {
	HTTP_Client_Init(&this->client, $("www.rfi.fr"));
}

def(void, Destroy) {
	HTTP_Client_Destroy(&this->client);
}

void ref(DestroyItem)(TranscribedPodcastItem *item) {
	String_Destroy(&item->podcast);
	String_Destroy(&item->transcript);
}

def(void, GetListing, String name, Listing *res) {
	if (!HTTP_Client_IsConnected(&this->client)) {
		HTTP_Client_Open(&this->client);
	}

	String path = String_Format($("/radiofr/podcast/%.xml"), name);

	try (&exc) {
		HTTP_Client_Request(&this->client, $("www.rfi.fr"), path);
		HTTP_Client_FetchResponse(&this->client);
	} finally {
		String_Destroy(&path);
	} tryEnd;

	String buf = HTTP_Client_Read(&this->client, 50 * 1024);

	String s;
	ssize_t pos = 0;

	while ((pos = String_Between(buf, pos, $("<item>"), $("</item>"), &s)) != String_NotFound) {
		Podcast item;

		item.title = String_Between(s,
			$("<title>"),
			$("</title>"));

		if (String_Contains(item.title, $("Script journal"))) {
			continue;
		}

		ssize_t pos = String_Find(item.title, $(" - "));

		if (pos != String_NotFound && pos > 2) {
			item.title = String_Slice(item.title, 0, pos);
		}

		item.title = String_Clone(String_Trim(item.title));

		item.id = String_Clone(String_Between(s,
			$("<guid isPermaLink=\"false\">"),
			$("</guid>")));

		Date_RFC822 date =
			Date_RFC822_Parse(
				String_Between(s,
					$("<pubDate>"),
					$("</pubDate>")));

		item.date = ((DateTime *) &date)->date;

		TranscribedPodcastItem *data = New(TranscribedPodcastItem);

		data->podcast = String_Clone(String_Between(s, $("<enclosure url=\""), $("\"")));

		String link = String_Between(s, $("<link>"), $("</link>"));

		if (String_Contains(link, $("questionnaires"))) {
			data->transcript = $("");
		} else {
			data->transcript = String_Clone(link);
		}

		item.data = data;

		Array_Push(res, item);
	}

	String_Destroy(&buf);
}

def(void, Fetch, DownloaderClass dl, Podcast item) {
	TranscribedPodcastItem *data = item.data;

	Downloader_Get(dl, item, data->podcast);

	if (data->transcript.len > 0) {
		if (!HTTP_Client_IsConnected(&this->client)) {
			HTTP_Client_Open(&this->client);
		}

		URL_Parts parts = URL_Parse(data->transcript);

		try (&exc) {
			HTTP_Client_Request(&this->client, parts.host, parts.path);
			HTTP_Client_FetchResponse(&this->client);
		} finally {
			URL_Parts_Destroy(&parts);
		} tryEnd;

		String resp = HTTP_Client_Read(&this->client, 128 * 1024);

		String transcript;

		if (String_Contains(data->transcript, $("articles/"))) {
			transcript =
				String_Clone(
					String_Between(resp,
						$("<div class=\"texte\">"),
						$("</div>")));

			String_ReplaceAll(&transcript, $("&nbsp;"),  $(" "));

			String_ReplaceAll(&transcript, $("</p><p>"), $("\n\n"));
			String_ReplaceAll(&transcript, $("</p>"),    $("\n"));

			String_ReplaceAll(&transcript, $("<i>"),     $(""));
			String_ReplaceAll(&transcript, $("</i>"),    $(""));

			String_ReplaceAll(&transcript, $("<b>"),     $(""));
			String_ReplaceAll(&transcript, $("</b>"),    $(""));

			String_ReplaceAll(&transcript, $("<sup>"),   $(""));
			String_ReplaceAll(&transcript, $("</sup>"),  $(""));

			String_ReplaceAll(&transcript, $("<br />"),  $("\n"));
		} else {
			String_Outside(&resp,
				$("<div class=\"article-main-authors\">"),
				$("</div>"));

			String tmp = HeapString(1024);

			String_Append(&tmp, String_Trim(String_Between(resp,
				$("<div class=\"edition-header-intro\">"),
				$("</div>"))));

			String_Append(&tmp, String_Trim(String_Between(resp,
				$("<div class=\"edition-main-text\">"),
				$("</div>"))));

			String_ReplaceAll(&tmp, $("<p><P>"), $(""));
			String_ReplaceAll(&tmp, $("<p><P class=\"MsoNormal\">"), $(""));

			String_ReplaceAll(&tmp, $("</p>"),   $(""));

			String_ReplaceAll(&tmp, $("<BR />"), $("\n"));

			String_ReplaceAll(&tmp, $(" :\n"),   $(": "));
			String_ReplaceAll(&tmp, $(" : \n"),  $(": "));

			String_ReplaceAll(&tmp, $("<I>"),    $(""));
			String_ReplaceAll(&tmp, $("</i>"),   $(""));

			String_ReplaceAll(&tmp, $("<B>"),    $(""));
			String_ReplaceAll(&tmp, $("</b>"),   $(""));

			String_ReplaceAll(&tmp, $("&nbsp;"), $(""));

			while(String_Outside(&tmp, $("&lt;"), $("&gt;")));

			/* See also:
			 * http://www.rfi.fr/emission/20100921-le-premier-mot-vassilis-alexakis
			 */

			String_ReplaceAll(&tmp, $("<em>"),      $(""));
			String_ReplaceAll(&tmp, $("</em>"),     $(""));
			String_ReplaceAll(&tmp, $("<p>"),       $("\n"));
			String_ReplaceAll(&tmp, $("</a>"),      $(""));
			String_ReplaceAll(&tmp, $("<br />"),    $("\n"));
			String_ReplaceAll(&tmp, $("<strong>"),  $(""));
			String_ReplaceAll(&tmp, $("</strong>"), $(""));
			transcript = HTML_Entities_Decode(String_Trim(tmp));

			String_Destroy(&tmp);
		}

		if (transcript.len > 128) {
			Downloader_SaveText(dl, item, transcript);
		}

		String_Destroy(&transcript);

		String_Destroy(&resp);
	}
}

ProviderInfo ref(Info) = {
	.id      = $("rfi"),
	.methods = {
		.size        = sizeof(self),
		.init        = (void *) ref(Init),
		.destroy     = (void *) ref(Destroy),
		.destroyItem = (void *) ref(DestroyItem),
		.getListing  = (void *) ref(GetListing),
		.fetch       = (void *) ref(Fetch)
	}
};
