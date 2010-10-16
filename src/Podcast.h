#import <Date.h>
#import <Array.h>
#import <String.h>

record {
	String id;
	String title;
	Date date;

	void *data;
} Class(Podcast);

typedef Array(Podcast, Listing);

// ---

record {
	String podcast;    /* Podcast URL */
} DefaultPodcastItem;

record {
	String podcast;    /* Podcast URL */
	String transcript; /* Transcript URL */
} TranscribedPodcastItem;
