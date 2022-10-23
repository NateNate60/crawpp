#pragma once
#include <nlohmann/json.hpp>
#include <string>

#include "crawpp/Redditor.h"
#include "crawpp/CRAWObject.h"
#include "crawpp/Award.hpp"

namespace CRAW {
    /**
    @brief Models a Reddit submission. Post and Comment
    inherit from this class.

    @warning This class should not be directly instantiated.
    */
    class Submission : public CRAWObject{
        private:
            /**
             * Cast a vote in a certain direction for a submission.
             * Users should use upvote(), downvote(), and clearvote()
             * instead.
             * 
             * @param direction 1 for upvote, -1 for downvote, 0 for unvote
             */
            void _vote (int direction);
        public:
            /**
             *  Stores information about the submission.
             * This stores the API response to avoid duplicate API calls.
             */
            nlohmann::json information;

            /**
			The ID assigned to the submission by Reddit
			*/
            std::string id;

            /**
			The username of the author of the submission
			*/
            std::string authorname;
            /** Fetches a Redditor instance for the author of the submission */
            Redditor author ();

            /**
			The fullname of the submission. The fullname is used by Reddit and is a combination of a thing's type and its globally-unique ID.
			*/
            std::string fullname;

            /**
			The type of submission, such as "link" or "text"
			*/
            std::string type;

            /**
             * The contents of the submission. This is the selftext for text posts, or the URL for image posts.
             * @note Image posts can now also contain selftext. Check Submission.selftext as well.
			*/
            std::string content;

            /**
             * The selftext of the submission.
             * @note Image posts can now also contain selftext. The image URL is in Submission.content.
             */
            std::string selftext;

            /**
			The name of the subreddit that the submission was made in. Use subreddit() instead to get a Subreddit instance
			*/
            std::string subredditname;

            /**
			The time that the submission was made
			*/
            time_t posted;

            /**
			The submission's current score. 
            @note The Reddit API doesn't return exact vote counts. This is known as "vote fuzzing".
			*/
            int score;

            /**
			The time that the submission was edited, or 0 if never
			*/
            time_t edited;

            /**
             * A list of awards that the submission has received. Duplicate
             * awards are not listed multiple times, instead incrementing the
             * "count" member variable of each award type.
             */
            std::vector<Award> awards;

            /**
            Reply to a submission. Returns the new comment.

            @param contents: The Markdown contents of the reply
            @param distinguish: Whether to distinguish as a moderator
            @return The Comment that was left as a reply
            */
            Comment reply (const std::string & contents, bool distinguish = false); 

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

            @param newcontents The new contents to replace the current submission contents
            @return A reference to the Submission that was edited
            @note The contents and the edited time will be updated, but nothing else
            will change in the Submission object.
            */
            Submission & edit (const std::string & newcontents);

            /**
            Upvote a submission. Returns a reference to the submission so
            calls can be chained together (e.g. Submission.reply().remove()).

            @note A human must to be the one triggering this action. Automated voting
            by bots is prohibited, however, proxying a human vote is allowed (i.e. it is
            okay to call a vote method when a human end user clicks a button to cast a
            single vote). Bots voting on their own or amplifying a human's vote is
            considered vote manipulation.

            @return A reference to the Submission that was upvoted
            */
            Submission & upvote ();

            /**
            Downvote a submission. Returns a reference to the submission so
            calls can be chained together (e.g. Submission.reply().remove()).

            @note A human must to be the one triggering this action. Automated voting
            by bots is prohibited, however, proxying a human vote is allowed (i.e. it is
            okay to call a vote method when a human end user clicks a button to cast a
            single vote). Bots voting on their own or amplifying a human's vote is
            considered vote manipulation.

            @return A reference to the Submission that was downvoted
            */
            Submission & downvote ();

            /**
            Clear your vote on a submission. Returns a reference to the submission so
            calls can be chained together (e.g. Submission.reply().remove()).

            @note A human must to be the one triggering this action. Automated voting
            by bots is prohibited, however, proxying a human vote is allowed (i.e. it is
            okay to call a vote method when a human end user clicks a button to cast a
            single vote). Bots voting on their own or amplifying a human's vote is
            considered vote manipulation.

            @return A reference to the Submission that was downvoted
            */
            Submission & clearvote ();
    };
}