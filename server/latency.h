#include <queue>
#include <chrono>


const int DELAY = 400;		// Milliseconds of delay

long long currentTime()
{
	return std::chrono::system_clock::now().time_since_epoch() / std::chrono::milliseconds(1);
}

struct info 
{
    long long timeToSend;
    int clientID;
    std::string msg;

    info(int _clientID, std::string _msg): 
                clientID(_clientID), msg(_msg)
    {
	    timeToSend = currentTime() + rand()%DELAY;
    }
};

struct NTP 
{
	long long timeToSend;           // The time that the server should send the message, it is not the actual sending time
	int clientID;
	int latencyID;
	long long X;

	NTP(int _clientID, int _latencyID, long long _X) : 
            clientID(_clientID), latencyID(_latencyID), X(_X)
	{
		timeToSend = currentTime() + rand()%DELAY;
	}
};

template<typename T>
struct greaterThanByTime {
	bool operator()(const T &lhs, const T &rhs){
		return lhs.timeToSend > rhs.timeToSend;
	}
};


std::priority_queue<info, vector<info>, greaterThanByTime<info>> lat;
std::priority_queue<NTP, vector<NTP>, greaterThanByTime<NTP>> lat_est;


