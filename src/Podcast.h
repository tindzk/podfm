#import <Date.h>
#import <Array.h>
#import <String.h>

typedef struct {
	String id;
	String title;
	Date date;

	void *data;
} Class(Podcast);

typedef Array(Podcast, Listing);

// ---

typedef struct {
	String podcast;    /* Podcast URL */
} DefaultPodcastItem;

typedef struct {
	String podcast;    /* Podcast URL */
	String transcript; /* Transcript URL */
} TranscribedPodcastItem;
