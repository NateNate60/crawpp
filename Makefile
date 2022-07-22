libcrawpp.so: main.o Reddit.o Redditor.o Subreddit.o Post.o Comment.o
	g++ -shared Reddit.o Redditor.o Subreddit.o Post.o Comment.o -o libcrawpp.so

Reddit.o: Reddit.cpp Reddit.h
	g++ -g -c -fPIC Reddit.cpp -lrestclient-cpp -lcurl

Redditor.o: Redditor.cpp Redditor.h
	g++ -g -c -fPIC Redditor.cpp -lrestclient-cpp -lcurl

Subreddit.o: Subreddit.cpp Subreddit.h
	g++ -g -c -fPIC Subreddit.cpp -lrestclient-cpp -lcurl

Post.o: Post.cpp Post.h
	g++ -g -c -fPIC Post.cpp -lrestclient-cpp -lcurl

Comment.o: Comment.cpp Comment.h
	g++ -g -c -fPIC Comment.cpp -lrestclient-cpp -lcurl