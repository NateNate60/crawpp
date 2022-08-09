#pragma once
#include <string>
#include <vector>
#include <ctime>
#include <nlohmann/json.hpp>
#include <string>

#include "crawpp/CRAWObject.h"
#include "crawpp/Reddit.h"
#include "crawpp/Rule.h"

namespace CRAW {

    /**
     * @brief A structure representing the options for a post, containing
     * information such as the flair or whether the post is NSFW or 
     * contains spoilers. Default values are sane.
     * 
     * @param ad Whether the post is an advert
     * @param collection_id The UUID of the collection to be associated with the post
     * @param event_start The local time the event starts
     * @param event_end The local time the event ends
     * @param event_tz The time zone of the event, such as "America/Los_Angeles"
     * @param flair_id The ID of the flair to use
     * @param flair_text The text to put in the flair (max 64 chars)
     * @param type "text" for text posts, "link" for everything else
     * @param resubmit 
     * @param inbox_replies Whether replies should be sent to the inbox
     * @param spoiler Whether to mark the post as containing spoilers
     */
    struct PostOptions {
        bool ad;
        std::string collection_id;
        time_t event_start;
        time_t event_end;
        std::string event_timezone;
        std::string flair_id;
        std::string flair_text;
        std::string type;
        bool nsfw;
        bool resubmit;
        bool get_inbox_replies;
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
            Fetches posts sorted in the specified way, up to the specified limit (default: 25 + pinned posts if sorting by hot)

            @param sort: How to sort the results (default: hot)
            @param period "hour", "day", "week", "month", "year", or "all". Used when sorting by top or controversial
            @param limit: How many posts to fetch (default: 25, max: 100)
            */
            std::vector<Post> posts (const std::string & sort = "hot",
                                     const std::string & period = "all",
                                     const unsigned short limit = 25);

            /**
             * @brief Make a new post on a subreddit. Returns the newly-made post as a Post instance
             * 
             * @param title The title of the post
             * @param contents The contents of the post
             * @param options A PostOptions struct containing the options for the post
             * @return Post instance of the newly-created post
             */
            Post post (const std::string & title,
                       const std::string & contents,
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