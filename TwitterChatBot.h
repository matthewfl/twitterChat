#ifndef _Twitter_ChatBot_h
#define _Twitter_ChatBot_h

#include <pthread.h>
#include <curl/curl.h>
#include <string>
#include <queue>
#include <vector>
#include <sqlite3.h>

class TwitterChatBot
{
private:
  sqlite3 * Tweet;
  sqlite3 * Know;
  std::string loginInfo;
  std::queue<std::string> tweetQueue;
  pthread_t Pfeed, PtweetData;
  CURL * curl;
  bool feedRunning;
  std::string tempHolder;
  static void * StreamThreadStart (void*);
  static size_t StreamCallBack (char*, size_t, size_t, TwitterChatBot*);
  void proccessStream (std::string &);
  static void * AddTweetData (void*);
  bool SystemRunning;
public:
  std::string feedUrl;

  TwitterChatBot (std::string user, std::string pass, sqlite3 * tweet, sqlite3 * know): Tweet(tweet), Know(know), loginInfo(user+":"+pass), feedRunning(false) { start(); };
  TwitterChatBot (std::string userpwd, sqlite3 * tweet, sqlite3 * know): Tweet(tweet), Know(know), loginInfo(userpwd), feedRunning(false) { start(); }
  ~TwitterChatBot () { stop(); }
  void startFeed();
  void stopFeed();
  void start();
  void stop();
  unsigned int queueSize();
};




#endif // _Twitter_ChatBot_h
