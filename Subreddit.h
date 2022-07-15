#pragma once
#include <string>
#include <vector>
#include <ctime>
#include <nlohmann/json.hpp>

#include "Reddit.h"

namespace CRAW {

    /**
    Represents a subreddit
    */
    class Subreddit {
        private:
            Reddit * _redditinstance;
        public:
            // Stores info about the subreddit
            nlohmann::json information;

            // The name of the subreddit, not including the r/
            std::string name;
            
            // The fullname of the subreddit, which always starts with "t5_". The fullname is used by Reddit and is a combination of a thing's type and its globally-unique ID.
            std::string fullname;

            // Whether the current user is banned (always false if not authenticated)
            bool banned;

            // Whether there are posting restrictions
            bool postingrestricted;

            // Whether the subreddit is quarantined
            bool quarantined;

            // The subreddit's language
            std::string language;

            // When the subreddit was created
            time_t created;

            // The number of subscribers (members) that the subreddit has
            unsigned int subscribers;

            // How many people on the subreddit are here now
            unsigned int activeusers;

            /**
            Initialises a new Subreddit instance assoicated with a Reddit instance.

            @param redditinstance: The Reddit instance to associate
            @param subredditname: The name of the subreddit
            */
            Subreddit (const std::string & subredditname, Reddit * redditinstance = nullptr);

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
            Make a new post on a subreddit. Returns the newly-made post as a Post instance.
            To set post flair, call flair() on the returned Post instance, e.g. 
            Subreddit.post().flair()

            @param title: The title of the post
            @param contents: The contents of the post
            @param distinguish: Whether to distinguish as a moderator
            @param pin: Whether to pin the post at the top of the subreddit
            */
            Post post (const std::string & title,
                       const std::string & contents,
                       bool distinguish,
                       bool pin);
            
            /**
            Subscribe to a subreddit. Returns a reference to the subreddit so that
            calls can be chained together
            */
            Subreddit & subscribe ();

            /**
            Ban a user from a subreddit.

            @param user The user to ban from the subreddit
            @param reason The reason for the ban
            @param length The length of the ban, in days (0 = permanant, max 999)
            @return A reference to the Subreddit instance for the subreddit that the user was just banned from
            */
            Subreddit & ban (const Redditor & user, std::string reason, unsigned short length);

            /**
             * Unbans a user from a subreddit.
             * 
             * @param user The user to unban from the subreddit
             * @return A reference to the Subreddit instance for the subreddit that the user was just banned form
             */
            Subreddit & unban (const Redditor & user);

    };
}