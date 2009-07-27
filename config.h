#ifndef _twitter_config_h
#define _twitter_config_h

// if using this for user in password comment out Twitter_login_args
#define Twitter_UserName "matthewfl"
#define Twitter_Password "no way"

#define Twitter_login_args

// what feed to use
#define Twitter_Feed_url "http://stream.twitter.com/spritzer.json"

// names of database
// databases should be inited before running the programs
#define Twitter_database_tweet "tweets.database"
#define Twitter_database_knowledge "know.database"

// the max size of all the queues
#define Twitter_Queue_maxsize 500
#define Twitter_Queue_checkTime_sec 5

#endif // _tiwtter_config_h

