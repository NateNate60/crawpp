#pragma once
#include <nlohmann/json.hpp>
#include <string>

#include "crawpp/Reddit.h"

namespace CRAW {
    /**
    Models a Reddit submission. Post and Comment
    inherit from this class.

    This class should not be directly instantiated.
    */
    class Submission {
        private:
            /**
             * Cast a vote in a certain direction for a submission.
             * Users should use upvote(), downvote(), and clearvote()
             * instead.
             * 
             * @param direction 1 for upvote, -1 for downvote, 0 for unvote
             */
            void _vote (int direction);
        protected:
            Reddit * _redditinstance;
        public:
            /**
             *  Stores information about the submission.
             * This stores the API response to avoid duplicate API calls.
             */
            nlohmann::json information;

            // The ID assigned to the submission by Reddit
            std::string id;

            // The username of the author of the submission
            std::string authorname;
            // Fetches a Redditor instance for the author of the submission
            Redditor author ();

            // The fullname of the submission. The fullname is used by Reddit and is a combination of a thing's type and its globally-unique ID.
            std::string fullname;

            // The type of submission, such as "link" or "text"
            std::string type;

            // The contents of the submission
            std::string content;

            // The name of the subreddit that the submission was made in. Use subreddit() instead to get a Subreddit instance
            std::string subredditname;

            // The time that the submission was made
            time_t posted;

            // The submission's current score. Note that due to the vote-fuzzing done by Reddit, this number is not 100% accurate.
            int score;

            // The time that the submission was edited, or 0 if never
            time_t edited;

            /**
            Reply to a submission. Returns the new comment.

            @param contents: The Markdown contents of the reply
            @param distinguish: Whether to distinguish as a moderator
            @return The Comment that was left as a reply
            */
            Comment reply (std::string & contents, bool distinguish = false); 

            /**
            Returns the subreddit that the submission was made in

            @return A Subreddit instance for the subreddit the submission was made in
            */
            Subreddit subreddit();

            /**
            Remove the submission (as a moderator).

            @param spam: Whether to mark the submission as spam, which trains the spam filter
            */
            void remove (bool spam = false);

            /**
            Delete the submission (as the author)
            */
            void del ();

            /**
            Edit the submission (as the author). Returns a reference to the submission so
            calls can be chained together (e.g. Submission.reply().remove()).

            @param newcontent: The new contents to replace the current submission contents
            @return A reference to the Submission that was edited
            @note The contents and the edited time will be updated, but nothing else
            will change in the Submission object.
            */
            Submission & edit (const std::string & newcontents);

            /**
            Upvote a submission. Returns a reference to the submission so
            calls can be chained together (e.g. Submission.reply().remove()).

            @note A human must to be the one triggering this action. Reddit prohibits 
            automated voting by bots as that is considered vote-cheating.

            @return A reference to the Submission that was upvoted
            */
            Submission & upvote ();

            /**
            Downvote a submission. Returns a reference to the submission so
            calls can be chained together (e.g. Submission.reply().remove()).

            @note A human must to be the one triggering this action. Reddit prohibits 
            automated voting by bots as that is considered vote-cheating.

            @return A reference to the Submission that was downvoted
            */
            Submission & downvote ();

            /**
            Clear your vote on a submission. Returns a reference to the submission so
            calls can be chained together (e.g. Submission.reply().remove()).

            @note A human must to be the one triggering this action. Reddit prohibits 
            automated voting by bots as that is considered vote-cheating.

            @return A reference to the Submission that was downvoted
            */
            Submission & clearvote ();
    };
}