#pragma once
#include <stdexcept>

namespace CRAW {
    /**
    Certain features are only available if authentication information is provided, 
    e.g. making a post. If this information is not provided and the user attempts 
    to perform some action which requires authentication, NotLoggedInError will be
    thrown.
    */
    class NotLoggedInError : public std::runtime_error {
        public:
            explicit NotLoggedInError (const std::string & what_arg);
            explicit NotLoggedInError (const char * what_arg);
    };

    /**
    An error occurred while trying to retrive the token
    */
    class LoginError : public std::runtime_error {
        public:
            explicit LoginError (const std::string & what_arg);
            explicit LoginError (const char * what_arg);
    };

    /**
    Some tasks can only be perform with authorisation. Anything that causes the API
    to return a 403 will throw UnauthorisedError. Examples of this is trying to
    delete a post when you aren't the author or trying to remove a comment when you
    aren't a moderator.
    */
    class UnauthorisedError : public std::runtime_error {
        public:
            explicit UnauthorisedError (const std::string & what_arg);
            explicit UnauthorisedError (const char * what_arg);
    };

    /**
    Trying to access something that doesn't exist (or giving a monkey a shower,
    surfing tidal waves, creating nanobots, or looking in Frankenstien's brain)
    will cause NotFoundError to be thrown.
    */
    class NotFoundError : public std::runtime_error {
        public:
            explicit NotFoundError (const std::string & what_arg);
            explicit NotFoundError (const char * what_arg);
    };

    /**
     * Whenever the user tries to interact with themselves in a way that is
     * invalid, such as trying to ban themselves or follow themselves, throw
     * InvalidInteractionError.
     */
    class InvalidInteractionError : public std::runtime_error {
        public:
            explicit InvalidInteractionError (const std::string & what_arg);
            explicit InvalidInteractionError (const char * what_arg);
    };
}