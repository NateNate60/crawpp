#pragma once
#include <string>
#include <ctime>
#include <nlohmann/json.hpp>

#include "Reddit.h"

namespace CRAW {
    /**
    Represents a Reddit user.
    */
    class Redditor {
        private:
            const Reddit * _redditinstance;
            nlohmann::json _information;
        public:
            // The user's username, without the u/
            std::string username;

            // The UTC time that this account was created
            time_t created;

            // The total number of karma points this user has
            unsigned long totalkarma;
            // The number of karma points earned from giving out awards
            unsigned long awarderkarma;
            // The number of karma points earned from getting awards
            unsigned long awardeekarma;
            // The number of karma points earned from posts
            unsigned long postkarma;
            // The number of karma points earned from comments
            unsigned long commentkarma;

            /**
            Initialise a Redditor instance matching the given username.
            
            @param name: The username of the Redditor
            */
            Redditor (const std::string & name, const Reddit * redditinstance = nullptr);

            /**
            The [] operator is used to fetch information about a user. All information is returned as a std::string.
            Some commonly-used attributes can be fetched using the dot operator (.), but all information can be fetched using this.
            If the value is an array or an object, it will be given as a JSON string.
            */
            std::string operator[] (const std::string & attribute);

            /** 
             * Follow this Redditor
            */
            void follow ();
            /** 
             * Unfollow this Redditor
            */
            void unfollow ();
    };
}