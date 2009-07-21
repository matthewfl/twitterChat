#include "TwitterChatBot.h"
#include <pthread.h>
#include <curl/curl.h>

#include <string>
#include <iostream>
using namespace std;

void TwitterChatBot::startFeed () {
  feedRunning = true;
  pthread_create(&Pfeed, NULL, StreamThreadStart, (void*)this);
}

void TwitterChatBot::stopFeed() {
  if(feedRunning) {
    pthread_cancel(Pfeed);
    curl_easy_pause(curl, CURLPAUSE_ALL);
    curl_easy_cleanup(curl);
    
    feedRunning = false;
  }
}

void * TwitterChatBot::StreamThreadStart(void * self) {
  TwitterChatBot * s = reinterpret_cast<TwitterChatBot*>(self);
  // open the socket
  cout<<"here\n";
  CURL * curl = curl_easy_init();
  s->curl = curl;
  CURLcode result;
  // login in info
  curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
  curl_easy_setopt(curl, CURLOPT_USERPWD, s->loginInfo.c_str());

  // other the url to use
  curl_easy_setopt(curl, CURLOPT_URL, s->feedUrl.c_str());

  // other info
  curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0 matthewfl.com-chatBot/1.0");
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
  curl_easy_setopt(curl, CURLOPT_HEADER, 0);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, s);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, StreamCallBack);

  cout << "preform\n";
  result = curl_easy_perform(curl);

  // thread done running
  s->feedRunning = false;

  // close the socket
  curl_easy_cleanup(curl);
  s->curl = NULL;

  return NULL; // what am I to return here?

}

size_t TwitterChatBot::StreamCallBack (char * str, size_t size, size_t nmemb, TwitterChatBot * self) {
  string s;
  s.append(str, size * nmemb);
  self->proccessStream(s);
  return size * nmemb;
}

void TwitterChatBot::proccessStream (string & data) {
  cout << data << endl << endl;
  matthewfl::json j;
  j.prase(data);
  

}
