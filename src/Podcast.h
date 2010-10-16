#import <Date.h>
#import <Array.h>
#import <String.h>

class(Podcast) {
	String id;
	String title;
	Date date;

	void *data;
};

typedef Array(Podcast, Listing);

// ---

record(DefaultPodcastItem) {
	String podcast;    /* Podcast URL */
};

record(TranscribedPodcastItem) {
	String podcast;    /* Podcast URL */
	String transcript; /* Transcript URL */
};
