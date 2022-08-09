#pragma once

namespace CRAW {
    class Reddit;

    /**
     * @brief A base class that all CRAW models inherit from.
     * @warning This class should not be instantiated on its own.
     * 
     */
    class CRAWObject {
        protected:
            /// The Reddit session associated with the object
            Reddit * _redditinstance;
    };
}