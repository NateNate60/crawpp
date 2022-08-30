VERSION = 0.2alpha-1

INCLUDEPATH = ./include
STANDARD = c++17
SOURCE = ./crawpp
OBJECTS = Reddit.o Redditor.o Subreddit.o Post.o Comment.o Submission.o
HEADERS = $(INCLUDE)/Award.hpp  $(INCLUDE)/Comment.h  $(INCLUDE)/crawexceptions.hpp  $(INCLUDE)/craw.h  $(INCLUDE)/Post.h  $(INCLUDE)/Reddit.h  $(INCLUDE)/Redditor.h  $(INCLUDE)/Submission.h  $(INCLUDE)/Subreddit.h
INCLUDE = $(INCLUDEPATH)/crawpp
EXEARGS = -g -I$(INCLUDEPATH) -L$(SOURCE) --std=$(STANDARD)
ARGS = -c $(EXEARGS)

PACKAGE = libcrawpp_$(VERSION)_amd64

libcrawpp.a: $(OBJECTS) $(INCLUDE)/crawexceptions.hpp
	ar crf libcrawpp.a $(OBJECTS)

Reddit.o: $(SOURCE)/Reddit.cpp $(INCLUDE)/Reddit.h $(INCLUDE)/crawexceptions.hpp
	g++ $(ARGS) $(SOURCE)/Reddit.cpp

Redditor.o: $(SOURCE)/Redditor.cpp $(INCLUDE)/Redditor.h $(INCLUDE)/crawexceptions.hpp
	g++ $(ARGS) $(SOURCE)/Redditor.cpp

Subreddit.o: $(SOURCE)/Subreddit.cpp $(INCLUDE)/Subreddit.h $(INCLUDE)/crawexceptions.hpp
	g++ $(ARGS) $(SOURCE)/Subreddit.cpp

Post.o: $(SOURCE)/Post.cpp $(INCLUDE)/Post.h $(INCLUDE)/crawexceptions.hpp
	g++ $(ARGS) $(SOURCE)/Post.cpp

Comment.o: $(SOURCE)/Comment.cpp $(INCLUDE)/Comment.h $(INCLUDE)/crawexceptions.hpp
	g++ $(ARGS) $(SOURCE)/Comment.cpp

Submission.o: $(SOURCE)/Submission.cpp $(INCLUDE)/Submission.h $(INCLUDE)/crawexceptions.hpp
	g++ $(ARGS) $(SOURCE)/Submission.cpp

a.out: test.cpp libcrawpp.a
	g++ $(EXEARGS) -L. test.cpp -lcrawpp -lcpr

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
	cp $(INCLUDEPATH) $(PACKAGE)/usr/local/ -r
	cp libcrawpp.a $(PACKAGE)/usr/local/lib
	mkdir $(PACKAGE)/DEBIAN
	cp control $(PACKAGE)/DEBIAN
	mkdir $(PACKAGE)/usr/share/doc/libcrawpp/ -p
	cp LICENSE $(PACKAGE)/usr/share/doc/libcrawpp/copyright
	cp postinst $(PACKAGE)/DEBIAN
	dpkg-deb --build --root-owner-group $(PACKAGE)
	rm $(PACKAGE) -r

docs:
	doxygen Doxyfile
	scp -qr doc/html/crawpp nate@natenate60.xyz:/var/www/html