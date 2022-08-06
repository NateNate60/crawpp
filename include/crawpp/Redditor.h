#pragma once
#include <string>
#include <ctime>
#include <nlohmann/json.hpp>

#include "crawpp/Reddit.h"

namespace CRAW {
    /**
    @brief Represents a Reddit user.
    */
    class Redditor {
        private:
            Reddit * _redditinstance;
        public:
            /**
			Stores information about the user
			*/
            nlohmann::json information;

            /**
			The user's fullname. The fullname is used internally by Reddit.
			*/
            std::string fullname;

            /**
			The user's username, without the u/
			*/
            std::string username;

            /**
			The UTC time that this account was created
			*/
            time_t created;

            /**
			The total number of karma points this user has
			*/
            unsigned long totalkarma;
            /**
			The number of karma points earned from giving out awards
			*/
            unsigned long awarderkarma;
            /**
			The number of karma points earned from getting awards
			*/
            unsigned long awardeekarma;
            /**
			The number of karma points earned from posts
			*/
            unsigned long postkarma;
            /**
			The number of karma points earned from comments
			*/
            unsigned long commentkarma;

            /**
            Initialise a Redditor instance matching the given username.
            
            @param name: The username of the Redditor
            @param redditinstance The Reddit instance to associate with the Redditor
            */
            Redditor (const std::string & name, Reddit * redditinstance = nullptr);

            /**
            The [] operator is used to fetch information about a user. All information is returned as a std::string.
            Some commonly-used attributes can be fetched using the dot operator (.), but all information can be fetched using this.
            If the value is an array or an object, it will be given as a JSON string.
            */
            std::string operator[] (const std::string & attribute);

            /** 
             * Follow this Redditor, which is the same as subscribing to their user subreddit
            */
            void follow ();
            /** 
             * Unfollow this Redditor
            */
            void unfollow ();
    };
}