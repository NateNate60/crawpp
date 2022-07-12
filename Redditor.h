#pragma once
#include <string>
#include <ctime>

#include "Reddit.h"

namespace CRAW {
    /**
    Represents a Reddit user.
    */
    class Redditor {
        private:
            Reddit * _redditinstance;
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
            unsigned long linkkarma;
            // The number of karma points earned from comments
            unsigned long commentkarma;

            /**
            Initialise a Redditor instance matching the given username.
            In most cases, where the ability to interact with the Redditor is needed, use Reddit.redditor() instead.
            
            @param name: The username of the Redditor
            */
            Redditor(const std::string & name);

            /**
            Initialise a Redditor instance matching the given username.
            In most cases, where the ability to interact with the Redditor is needed, use Reddit.redditor() instead.
            
            @param name: The username of the Redditor
            */
            Redditor(const Reddit * redditinstance, const std::string & name);

            /**
            The [] operator is used to fetch information about a user. All information is returned as a std::string.
            Some commonly-used attributes can be fetched using the dot operator (.), but all information can be fetched using this.
            */
            std::string operator[] (const std::string & attribute);

            /*
            Returns whether two Redditor are the same (i.e. have the same name)
            */
            bool operator== (const Redditor & redditor);

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