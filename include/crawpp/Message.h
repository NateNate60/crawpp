#pragma once

#include <string>
#include <vector>
#include <ctime>

#include "crawpp/CRAWObject.h"
#include "crawpp/Redditor.h"

namespace CRAW {

    /**
     * Represents an object in a user's inbox
     * 
     * @warning Do not directly instantiate this class. Instead, it should only be obtained through Reddit.inbox().
     */
    class Message : public CRAWObject {
        public:

            /**
             * Whether the message is unread or has already been marked as read.
            */
            bool read;

            /**
             *  THe name of the subreddit this message originates from, without the r/ (empty string for private messages)
             */
            std::string subredditname;

            /**
             * The username of the author (sender) of the message, without the u/
             */
            std::string authorname;
            /**
             * Returns a Redditor instance of the author of the message
             * @return Redditor instance representing the author of the message
            */
            Redditor author ();

            /**
             * The score of the object that caused this message, such as the comment that was left in reply
             */
            int score;

            /**
             *  The unique ID assigned to this message
             */
            std::string id;

            /**
             * The type of message this is, either "pm", "post_reply", "comment_reply", or "modmail"
             */
            std::string type;

            /**
             * The subject of the message
             */
            std::string subject;

            /**
             * The body (contents) of the message
             */
            std::string body;

            /**
             * The fullname of this message
             */
            std::string fullname;

            /**
             * The time that this message was sent
             */
            time_t created;

            /**
             * The children of this message (i.e. the replies to a private message)
             * 
             * @note Only the root message 
            */
            std::vector<Message> children;

            /**
             * Initialise a new Message instance with the given data
             * @param data The JSON "data" field of the response
            */
            Message (const nlohmann::json & data, Reddit * redditinstance);

            /**
             * Reply to a message.
             * 
             * @param contents The Markdown contents of the message
             */
            void reply (const std::string & contents);

            /**
             * Mark this message as "read".
             */
            void mark_read();

            /**
             * Mark this message as "unread".
             */
            void mark_unread();
    };
}