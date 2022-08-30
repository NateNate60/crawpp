#pragma once
#include <string>
#include <vector>
#include <ctime>
#include <nlohmann/json.hpp>
#include <string>

#include "crawpp/CRAWObject.h"
#include "crawpp/Reddit.h"
#include "crawpp/Rule.h"
#include "crawpp/ListingPage.hpp"

namespace CRAW {

    /**
     * @brief A structure representing the options for a post.
     * 
     * This structure contains information such as the flair or 
     * whether the post is NSFW or contains spoilers. The default 
     * values are sane.
     */
    struct PostOptions {
        /// Whether the post is an advert (default: false)
        bool ad;

        /// The UUID of the collection to be associated with the post (default: "")
        std::string collection_id;

        /// The local time the event starts (default: 0, which is none)
        time_t event_start;

        /// The local time the event ends (default: 0, which is none)
        time_t event_end;

        /// The pytz time zone of the event, such as "America/Los_Angeles" (default: "", which is none)
        std::string event_timezone;

        /// The ID of the flair to use (default: "", which is none)
        std::string flair_id;

        /// The text to put in the flair (max 64 chars, default: "")
        std::string flair_text;

        /// "text" for text posts, "link" for everything else (default: "text")
        std::string type;
        
        /// Whether the post is not safe for work (e.g. contains gore or nudity, default: false)
        bool nsfw;

        /// Whether the post is a resubmit (default: false)
        bool resubmit;

        /// Whether replies should be sent to the inbox (default: true)
        bool get_inbox_replies;

        /// Whether the post contains spoilers (e.g. plot details of a newly-released film, default: false)
        bool spoiler;

        PostOptions () {
            ad = false;
            collection_id = "";
            event_start = 0;
            event_end = 0;
            event_timezone = "";
            flair_id = "";
            flair_text = "";
            type = "text";
            nsfw = false;
            resubmit = false;
            get_inbox_replies = true;
            spoiler = false;
        }
    };

    /**
    @brief Represents a subreddit
    */
    class Subreddit : public CRAWObject{
        private:
            /**
             * @brief Upload media to Reddit.
             * 
             * This API method was not documented by Reddit. The implementation used here
             * mirrors that of PRAW, the Python Reddit API Wrapper (BSD 2-clause license).
             * 
             * @param mediapath The path to some media to upload, such as
             * an image or video.
             * @param caption A caption to associate with the media (default: "") 
             * 
             * @note The media path must use forward slashes (/), not backward slashes (\).
             * @warning The media path can't contain non-ASCII characters.
             * 
             * @return A string containing the uploaded image's URL
             */
            std::string _upload (const std::string & mediapath, const std::string & caption = "");

        public:
            /**
			Stores info about the subreddit
			*/
            nlohmann::json information;

            /**
			The name of the subreddit, not including the r/
			*/
            std::string name;
            
            /**
			The fullname of the subreddit, which always starts with "t5_". The fullname is used by Reddit and is a combination of a thing's type and its globally-unique ID.
			*/
            std::string fullname;

            /**
			Whether the current user is banned (always false if not authenticated)
			*/
            bool banned;

            /**
			Whether there are posting restrictions
			*/
            bool postingrestricted;

            /**
			Whether the subreddit is quarantined
			*/
            bool quarantined;

            /**
			The subreddit's language
			*/
            std::string language;

            /**
			When the subreddit was created
			*/
            time_t created;

            /**
			The number of subscribers (members) that the subreddit has
			*/
            int subscribers;

            /**
			How many people on the subreddit are here now
			*/
            int activeusers;

            /**
            Initialises a new Subreddit instance assoicated with a Reddit instance.

            @param redditinstance: The Reddit instance to associate
            @param subredditname: The name of the subreddit
            */
            Subreddit (const std::string & subredditname, Reddit * redditinstance);

            /**
            The [] operator is used to fetch information about a subreddit. All information is returned as a std::string.
            Some commonly-used attributes can be fetched using the dot operator (.), but all information can be fetched using this.
            */
            std::string operator[] (const std::string & attribute);

            /**
            Fetches posts sorted in the specified way, up to the specified limit (default: 25 + pinned posts if sorting by hot).

            If desired, it is possible to flip through pages of listings by passing in a pointer to a ListingPage struct to the
            listingpage argument, and then specifying whether to return the page after that page or before that page in the direction
            argument.

            @param sort: How to sort the results (default: hot)
            @param period "hour", "day", "week", "month", "year", or "all". Used when sorting by top or controversial
            @param limit: How many posts to fetch (default: 25, max: 100)
            @param listingpage A pointer to a ListingPage struct, which can be used to flip forwards/backwards through pages of
            listings (defaut: nullptr, which means the first page). If provided, then the ListingPage for the page returned will
            be stored at whatever listingpage points to. To return the first page, initialise a blank ListingPage object and pass
            the address of that. The ListingPage will also be updated to refer to the first page, so that the second page can be
            retrieved by passing it to another posts() call.
            @param direction Whether to return the page after the page provided in listingpage, or the page before
            (either "after" or "before", default: "after"). Ignored if listingpage is nullptr.
            */
            std::vector<Post> posts (const std::string & sort = "hot",
                                     const std::string & period = "all",
                                     const int limit = 25,
                                     ListingPage * listingpage = nullptr,
                                     const std::string & direction = "after");

            /**
             * @brief Make a new post on a subreddit. Returns the newly-made post as a Post instance
             * 
             * @param title The title of the post
             * @param contents The contents of the post (Markdown text for text posts, 
             * a URL for link posts, or a path to some media for image or video posts)
             * @param type The type of post, either "link" or "text" (default: "text").
             * @param options A PostOptions struct containing the options for the post
             * @return Post instance of the newly-created post
             * 
             * @note To create a media (image or video) post, use postmedia(). You can 
             * still make posts that link to media using this method, however, to 
             * upload media to Reddit, use postmedia() instead. 
             */
            Post post (const std::string & title,
                       const std::string & contents,
                       const std::string & type = "text",
                       const PostOptions & options = PostOptions());
            

            /**
             * @brief Upload media and make a new post on a subreddit.
             * 
             * @param title The title of the post
             * @param contents The filepath to the content of the post
             * @param type The type of the post, either "image" or "video" (default: "image").
             * @param options A PostOptions struct containing the options for the post
             * 
             * @note To make a text or link post, use post() instead. Only use this message if
             * media needs to be uploaded. Posts that merely link to some media (like a YouTube
             * video) should be made with post().
             */
            void postmedia (const std::string & title,
                            const std::string & contents,
                            const std::string & type = "image",
                            const PostOptions & options = PostOptions());
            
            /**
            Subscribe to a subreddit. Returns a reference to the subreddit so that
            calls can be chained together

            @param skip_initial_defaults New accounts are automatically subscribed to a few popular
            subreddits. Should this behaviour be suppressed? (default: true)
            */
            Subreddit & subscribe (bool skip_initial_defaults = true);

            /**
             * @brief Unsubscribe from a subreddit
             * 
             * @return Subreddit& A reference to the Subreddit instance for the subreddit
             */
            Subreddit & unsubscribe ();

            /**
            Ban a user from a subreddit.

            @param user The user to ban from the subreddit
            @param message A message to send to the user
            @param length The length of the ban, in days (0 = permanant, max 999)
            @param reason The reason for the ban (e.g. "claiming DC is better than Marvel")
            @param modnote A note to leave for the other mods (max 300 characters)
            @return A reference to the Subreddit instance for the subreddit that the user was just banned from
            */
            Subreddit & ban (const Redditor & user,
                             const std::string & message,
                             int length,
                             const std::string & reason,
                             const std::string & modnote = "");
            
            /**
            Ban a user from a subreddit.

            @param username The username of the user to ban from the subreddit
            @param message A message to send to the user
            @param length The length of the ban, in days (0 = permanant, max 999)
            @param reason The reason for the ban (e.g. "claiming Marvel is better than DC")
            @param modnote A note to leave for the other mods (max 300 characters)
            @return A reference to the Subreddit instance for the subreddit that the user was just banned from
            */
            Subreddit & ban (const std::string & username,
                             const std::string & message,
                             int length,
                             const std::string & reason,
                             const std::string & modnote = "");
            
            
            /**
             * Unbans a user from a subreddit.
             * 
             * @param user The user to unban from the subreddit
             * @return A reference to the Subreddit instance for the subreddit that the user was just banned form
             */
            Subreddit & unban (const Redditor & user);

            /**
             * Unbans a user from a subreddit.
             * 
             * @param username The username of the user to unban from the subreddit
             * @param fullname The fullname of the user to be unbanned. If not provided, then it will be fetched
             * based on the username.
             * @return A reference to the Subreddit instance for the subreddit that the user was just banned form
             */
            Subreddit & unban (const std::string & username, const std::string & fullname = "");


            /**
             * @brief Fetch the subreddit rules
             * 
             * @return std::vector<Rule> The rules of the subreddit
             */
            std::vector<Rule> rules ();
    };
}