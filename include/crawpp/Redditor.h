#pragma once
#include <string>
#include <ctime>
#include <nlohmann/json.hpp>

#include "crawpp/Reddit.h"
#include "crawpp/CRAWObject.h"

namespace CRAW {
    /**
    @brief Represents a Reddit user.
    */
    class Redditor : public CRAWObject {
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
            int totalkarma;
            /**
			The number of karma points earned from giving out awards
			*/
            int awarderkarma;
            /**
			The number of karma points earned from getting awards
			*/
            int awardeekarma;
            /**
			The number of karma points earned from posts
			*/
            int postkarma;
            /**
			The number of karma points earned from comments
			*/
            int commentkarma;

            /**
            Initialise a Redditor instance matching the given username.
            
            @param name: The username of the Redditor
            @param redditinstance The Reddit instance to associate with the Redditor
            */
            Redditor (const std::string & name, Reddit * redditinstance);

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

            /**
             * @brief Block this Redditor (non-functional).
             * 
             * @note Attempting to view a user's profile after they've been blocked will result
             * in AuthorisationError. This behaviour is part of Reddit.
             * 
             * @warning Due to limitations in the scope of the token issued by Reddit this method currently
             * does not work.
             */
            void block ();
    };
}