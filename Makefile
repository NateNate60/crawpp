VERSION = 0.1alpha-1

INCLUDEPATH = ./include
STANDARD = c++17
SOURCE = ./crawpp
OBJECTS = Reddit.o Redditor.o Subreddit.o Post.o Comment.o
HEADERS = $(INCLUDE)/Award.h  $(INCLUDE)/Comment.h  $(INCLUDE)/crawexceptions.hpp  $(INCLUDE)/craw.h  $(INCLUDE)/Post.h  $(INCLUDE)/Reddit.h  $(INCLUDE)/Redditor.h  $(INCLUDE)/Submission.h  $(INCLUDE)/Subreddit.h
INCLUDE = $(INCLUDEPATH)/crawpp
ARGS = -g -c -I$(INCLUDEPATH) -L$(SOURCE) --std=$(STANDARD)

PACKAGE = libcrawpp_$(VERSION)_amd64

libcrawpp.a: $(OBJECTS) $(INCLUDE)/crawexceptions.hpp
	ar crf libcrawpp.a $(OBJECTS)

Reddit.o: $(SOURCE)/Reddit.cpp $(INCLUDE)/Reddit.h $(INCLUDE)/crawexceptions.hpp
	g++ $(ARGS) $(SOURCE)/Reddit.cpp -lcpr

Redditor.o: $(SOURCE)/Redditor.cpp $(INCLUDE)/Redditor.h $(INCLUDE)/crawexceptions.hpp
	g++ $(ARGS) $(SOURCE)/Redditor.cpp -lcpr

Subreddit.o: $(SOURCE)/Subreddit.cpp $(INCLUDE)/Subreddit.h $(INCLUDE)/crawexceptions.hpp
	g++ $(ARGS) $(SOURCE)/Subreddit.cpp -lcpr

Post.o: $(SOURCE)/Post.cpp $(INCLUDE)/Post.h $(INCLUDE)/crawexceptions.hpp
	g++ $(ARGS) $(SOURCE)/Post.cpp -lcpr

Comment.o: $(SOURCE)/Comment.cpp $(INCLUDE)/Comment.h $(INCLUDE)/crawexceptions.hpp
	g++ $(ARGS) $(SOURCE)/Comment.cpp -lcpr

a.out: test.cpp libcrawpp.a
	g++ $(ARGS) test.cpp -lcrawpp -lcpr

install: libcrawpp.a
	cp libcrawpp.a /usr/local/lib
	mkdir /usr/local/include/crawpp
	cp Reddit.h Redditor.h Subreddit.h Post.h Comment.h craw.h /usr/local/include/crawpp
	ldconfig
	echo "Done"

remove:
	rm /usr/local/include/crawpp -r
	rm /usr/local/lib/libcrawpp.a
	ldconfig
	echo "Done"

debpackage: libcrawpp.a $(HEADERS)
	mkdir $(PACKAGE)
	mkdir $(PACKAGE)/usr/local/lib -p
	cp $(INCLUDE) $(PACKAGE)/usr/local/ -r
	cp libcrawpp.a $(PACKAGE)/usr/local/lib
	mkdir $(PACKAGE)/DEBIAN
	cp control $(PACKAGE)/DEBIAN
	dpkg-deb --build --root-owner-group $(PACKAGE)
	rm $(PACKAGE) -r