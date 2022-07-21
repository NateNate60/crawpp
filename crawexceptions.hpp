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
     * Whenever the user tries to interact with something in a way that is
     * invalid, such as trying to ban themselves or edit an image post, throw
     * InvalidInteractionError.
     */
    class InvalidInteractionError : public std::runtime_error {
        public:
            explicit InvalidInteractionError (const std::string & what_arg);
            explicit InvalidInteractionError (const char * what_arg);
    };

    /**
     * This error should be thrown when the user attempts to edit something
     * that can't be edited, like someone else's post or an image post.
    */
    class EditingError : public InvalidInteractionError {
        public:
            explicit EditingError (const std::string & what_arg);
            explicit EditingError (const char * what_arg);
    };

    /**
     * RetrievalError represents a failed attempt to fetch info from the
     * server. Whenever the server either response with something in an
     * unexpected form or an error, RetrievalError will be thrown.
     * 
     * This is the most generic form of retrieval error. Use more specific
     * exceptions when the cause is actually known.
     */
    class RetrievalError : public std::runtime_error {
        public:
            explicit RetrievalError (const std::string & what_arg);
            explicit RetrievalError (const char * what_arg);
    };
}