#pragma once
#include <ctime>
#include <vector>
#include <nlohmann/json.hpp>

#include "Reddit.h"

namespace CRAW {

    /**
    An object representing a Reddit post
    */
    class Post {
        private:
            Reddit * _redditinstance;
        public:
            // The ID assigned to the post by Reddit
            std::string id;

            // The username of the author of the post
            std::string authorname;
            // Fetches a Redditor instance for the author of the post
            Redditor author ();

            // The fullname of the post. The fullname is used by Reddit and is a combination of a thing's type and its globally-unique ID.
            std::string fullname;

            // The type of post, such as "link" or "text"
            std::string type;

            // The title of the post
            std::string title;

            // The time that the post was made
            time_t posted;

            // The posts's current score. Note that due to the vote-fuzzing done by Reddit, this number is not 100% accurate.
            unsigned int score;

            // The text of the post's flair
            std::string flairtext;

            // The contents of the post
            std::string content;

            /**
            Fetch a post with the given ID that can only be viewed.
            In most cases, Reddit.post() should be used instead.

            @param id: The ID of the post
            */ 
            Post (const std::string & id);

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
            Post (Reddit * redditinstance, const std::string & id);

            // Returns whether two posts are the same (i.e. have the same id)
            bool operator== (const Post & post);

            /**
            Returns the subreddit that the post was posted in

            @return A Subreddit instance for the subreddit the post was made in
            */
            Subreddit parentsubreddit();

            /**
            Fetches the top-level comments of a post, sorted in the specified way, up to the specified limit

            @param sort: How to sort the comments (default: hot)
            @param limit: How many comments to fetch (default: 50)
            @return An std::vector of Comment objects, sorted in the specified way
            */
            std::vector<Comment> comments (const std::string & sort, const unsigned int limit);

            /**
            Make a new comment on a post.

            @param contents: The Markdown contents of the comment.
            @param distinguish: Whether to distinguish as a moderator
            @param pin: Whether to pin the comment to the top (will be distinguished as mod)
            @return The new comment that was left on the post
            */
            Comment comment (const std::string & contents, bool distinguish = false, bool pin = false);

            /**
            Remove the post (as a moderator).

            @param spam: Whether to mark the post as spam, which trains the spam filter
            @param reason: A removal message which will be pinned to the top of the post
            */
            void remove (bool spam = false, const std::string & reason = "");

            /**
            Delete the post (as the author).
            This would be called "delete" if that weren't a reserved keyword in C++.
            */
            void deletepost ();

            /**
            Edit the post (as the author). Returns a reference to the post so
            calls can be chained together (e.g. comment.reply().remove()).

            @param newcontent: The new contents to replace the current post contents
            */
            Post & edit (const std::string & newcontents);

            /**
            Upvote a post. Returns a reference to the post so
            calls can be chained together (e.g. comment.reply().remove()).

            @note A human must to be the one triggering this action. Reddit prohibits automated voting by bots as that is considered vote-cheating.
            */
            Post & upvote ();

            /**
            Downvote a post. Returns a reference to the post so
            calls can be chained together (e.g. comment.reply().remove()).

            @note A human must to be the one triggering this action. Reddit prohibits automated voting by bots as that is considered vote-cheating.
            */
            Post & downvote ();


    };
}