#pragma once
#include <ctime>
#include <vector>
#include <nlohmann/json.hpp>

#include "crawpp/Reddit.h"
#include "crawpp/Submission.h"

namespace CRAW {

    /**
    An object representing a Reddit post
    */
    class Post : public Submission {
        private:
            /**
             * Initialise this Post instance with the given data
             * 
             * @param data The "data" field of the Reddit API response
             * @param comments The "children" field of the COMMENTS listing, if any.
             * Saves an API call if this data was also given when the post data was
             * fetched
             */
            void _init (const nlohmann::json & data, const nlohmann::json & comments = nlohmann::json());

            // Stores the comments data from the post, which is the "children" field of a comment listing
            nlohmann::json _comments;
        public:

            // The text of the post's flair
            std::string flairtext;

            // The title of the post
            std::string title;

            /**
             * Construct a new empty Post object with the given information, in the
             * form of a Reddit API call response. This constructor expects the "data"
             * object within the response. Use this to avoid making a duplicate API
             * call.
             * 
             * @param redditinstance The Reddit instance to associate with the post
             */
            Post (nlohmann::json & data, Reddit * redditinstance = nullptr);

            /**
            Fetch a post with the given ID, associated with a given Reddit instance.

            @param redditinstance: The Reddit instance to associate
            @param id: The ID of the post to fetch
            */
            Post (const std::string & id, Reddit * redditinstance = nullptr);

            /**
            Fetches the top-level comments of a post, sorted in the specified way, up to the specified limit

            @param sort: How to sort the comments (default: hot)
            @param limit: How many comments to fetch (default: 50)
            @return An std::vector of Comment objects, sorted in the specified way
            */
            std::vector<Comment> comments (const std::string & sort, const unsigned int limit = 25);
    };
}