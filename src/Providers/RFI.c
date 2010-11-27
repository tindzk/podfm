#import <URL.h>
#import <Date.h>
#import <HTTP/Client.h>
#import <Date/RFC822.h>
#import <HTML/Entities.h>

#import "../ProviderInterface.h"

#undef self
#define self Providers_RFI

class(self) {
	bool inclDate;
	HTTP_Client client;
};

extern ExceptionManager exc;

def(void, Init) {
	this->inclDate = true;
	HTTP_Client_Init(&this->client, $("www.rfi.fr"));
}

def(void, Destroy) {
	HTTP_Client_Destroy(&this->client);
}

def(bool, SetOption, String key, String value) {
	if (String_Equals(key, $("date"))) {
		this->inclDate = String_Equals(value, $("yes"));
		return true;
	}

	return false;
}

sdef(void, DestroyItem, TranscribedPodcastItem *item) {
	String_Destroy(&item->podcast);
	String_Destroy(&item->transcript);
}

def(void, GetListing, String name, Listing **res) {
	String path = String_Format($("/radiofr/podcast/%.xml"), name);

	try {
		if (!HTTP_Client_IsConnected(&this->client)) {
			HTTP_Client_Open(&this->client);
		}

		HTTP_Client_Request(&this->client, $("www.rfi.fr"), path);
		HTTP_Client_FetchResponse(&this->client);
	} clean finally {
		String_Destroy(&path);
	} tryEnd;

	String buf = HTTP_Client_Read(&this->client, 50 * 1024);

	String s;
	ssize_t pos = 0;

	while ((pos = String_Between(buf, pos, $("<item>"), $("</item>"), &s)) != String_NotFound) {
		String title = String_Between(s,
			$("<title>"),
			$("</title>"));

		if (String_Contains(title, $("Script journal"))) {
			continue;
		}

		ssize_t pos = String_Find(title, $(" - "));

		if (pos != String_NotFound && pos > 2) {
			title = String_Slice(title, 0, pos);
		}

		title = String_Trim(title);

		String id = String_Between(s,
			$("<guid isPermaLink=\"false\">"),
			$("</guid>"));

		Date_RFC822 rfcDate =
			Date_RFC822_Parse(
				String_Between(s,
					$("<pubDate>"),
					$("</pubDate>")));

		TranscribedPodcastItem *item = New(TranscribedPodcastItem);

		item->base = (ListingItem) {
			.id    = String_Clone(id),
			.title = title
		};

		item->date    = rfcDate.date;
		item->podcast = String_Clone(
			String_Between(s,
				$("<enclosure url=\""),
				$("\"")));

		String link = String_Between(s,
			$("<link>"),
			$("</link>"));

		if (String_Contains(link, $("questionnaires"))) {
			item->transcript = $("");
		} else {
			item->transcript = String_Clone(link);
		}

		Listing_Push(res, (ListingItem *) item);
	}

	String_Destroy(&buf);
}

def(void, Fetch, DownloaderInstance dl, ListingItem *item) {
	TranscribedPodcastItem *data = (TranscribedPodcastItem *) item;

	String prefix = HeapString(0);

	if (this->inclDate) {
		prefix = Utils_GetDate(data->date);
	}

	Downloader_Get(dl, prefix, item, data->podcast);

	if (data->transcript.len > 0) {
		if (!HTTP_Client_IsConnected(&this->client)) {
			HTTP_Client_Open(&this->client);
		}

		URL_Parts parts = URL_Parse(data->transcript);

		try {
			HTTP_Client_Request(&this->client, parts.host, parts.path);
			HTTP_Client_FetchResponse(&this->client);
		} clean finally {
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
			Downloader_SaveText(dl, prefix, item, transcript);
		}

		String_Destroy(&transcript);

		String_Destroy(&resp);
	}

	String_Destroy(&prefix);
}

ProviderInterface ref(ProviderImpl) = {
	.id          = $("rfi"),
	.size        = sizeof(self),
	.init        = (void *) ref(Init),
	.destroy     = (void *) ref(Destroy),
	.destroyItem = (void *) ref(DestroyItem),
	.setOption   = (void *) ref(SetOption),
	.getListing  = (void *) ref(GetListing),
	.fetch       = (void *) ref(Fetch)
};
