#import <Date.h>
#import <Array.h>
#import <String.h>

record(ListingItem) {
	String id;
	String title;
};

Array(ListingItem *, Listing);

// ---

record(DefaultPodcastItem) {
	ListingItem base;

	Date date;
	String podcast;    /* Podcast URL */
};

record(TranscribedPodcastItem) {
	ListingItem base;

	Date date;
	String podcast;    /* Podcast URL */
	String transcript; /* Transcript URL */
};
