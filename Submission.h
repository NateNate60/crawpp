#include <nlohmann/json.hpp>

#include "Reddit.h"

namespace CRAW {
    /**
    Models a Reddit submission. Post and Comment
    inherit from this class.

    This class is virtual and cannot be instantiated.
    */
    class Submission {
        protected:
            Reddit * _redditinstance;
        public:
            // Stores information about the post
            nlohmann::json information;

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

            // The contents of the post
            std::string content;

            // The name of the subreddit that the post was made in. Use parentsubreddit() instead to get a Subreddit instance
            std::string subreddit;

            // The time that the post was made
            time_t posted;

            // The posts's current score. Note that due to the vote-fuzzing done by Reddit, this number is not 100% accurate.
            unsigned int score;

            // Pure virtual destructor to make this class abstract
            virtual ~Submission() = 0;

            /**
            Reply to a submission. Returns the new comment.

            @param contents: The Markdown contents of the reply
            @param distinguish: Whether to distinguish as a moderator
            @return The Comment that was left as a reply
            */
            Comment reply (std::string contents, bool distinguish = false); 

            /**
            Returns the subreddit that the submission was made in

            @return A Subreddit instance for the subreddit the submission was made in
            */
            Subreddit parentsubreddit();

            /**
            Remove the submission (as a moderator).

            @param spam: Whether to mark the submission as spam, which trains the spam filter
            */
            void remove (bool spam = false);

            /**
            Delete the submission (as the author)
            This would be called "delete" if that weren't a reserved keyword in C++.
            */
            void deletesubmission ();

            /**
            Edit the submission (as the author). Returns a reference to the submission so
            calls can be chained together (e.g. Submission.reply().remove()).

            @param newcontent: The new contents to replace the current submission contents
            @return A reference to the Submission that was edited
            */
            Submission & edit (const std::string & newcontents);

            /**
            Upvote a submission. Returns a reference to the submission so
            calls can be chained together (e.g. Submission.reply().remove()).

            Note: A human must to be the one triggering this action. Reddit prohibits 
            automated voting by bots as that is considered vote-cheating.

            @return A reference to the Submission that was upvoted
            */
            Submission & upvote ();

            /**
            Downvote a submission. Returns a reference to the submission so
            calls can be chained together (e.g. Submission.reply().remove()).

            Note: A human must to be the one triggering this action. Reddit prohibits 
            automated voting by bots as that is considered vote-cheating.

            @return A reference to the Submission that was downvoted
            */
            Submission & downvote ();
    };
}