#ifndef _Twitter_ChatBot_h
#define _Twitter_ChatBot_h

#include <pthread.h>
#include <curl/curl.h>
#include <string>

class TwitterChatBot
{
private:
  std::string loginInfo;
  pthread_t Pfeed;
  CURL * curl;
  bool feedRunning;
  static void * StreamThreadStart (void*);
  static size_t StreamCallBack (char*, size_t, size_t, TwitterChatBot*);
  void proccessStream (std::string &);
public:
  std::string feedUrl;

  TwitterChatBot (std::string user, std::string pass): loginInfo(user+":"+pass), feedRunning(false) {};
  TwitterChatBot (std::string userpwd): loginInfo(userpwd), feedRunning(false) {}
  void startFeed();
  void stopFeed();
};




#endif // _Twitter_ChatBot_h
