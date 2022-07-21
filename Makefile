a.out: main.o Reddit.o Redditor.o Subreddit.o Post.o Comment.o
	g++ main.o Reddit.o Redditor.o Subreddit.o Post.o Comment.o -lrestclient-cpp -lcurl

main.o: main.cpp
	g++ -g -c main.cpp

Reddit.o: Reddit.cpp Reddit.h
	g++ -g -c Reddit.cpp -lrestclient-cpp -lcurl

Redditor.o: Redditor.cpp Redditor.h
	g++ -g -c Redditor.cpp -lrestclient-cpp -lcurl

Subreddit.o: Subreddit.cpp Subreddit.h
	g++ -g -c Subreddit.cpp -lrestclient-cpp -lcurl

Post.o: Post.cpp Post.h
	g++ -g -c Post.cpp -lrestclient-cpp -lcurl

Comment.o: Comment.cpp Comment.h
	g++ -g -c Comment.cpp -lrestclient-cpp -lcurl