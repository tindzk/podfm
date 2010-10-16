#import <Date.h>
#import <Array.h>
#import <String.h>

Class(Podcast) {
	String id;
	String title;
	Date date;

	void *data;
};

typedef Array(Podcast, Listing);

// ---

record {
	String podcast;    /* Podcast URL */
} DefaultPodcastItem;

record {
	String podcast;    /* Podcast URL */
	String transcript; /* Transcript URL */
} TranscribedPodcastItem;
