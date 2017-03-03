#include <queue>
#include <chrono>

const int DELAY = 300;		// Milliseconds of delay

long long currentTime()
{
	return std::chrono::system_clock::now().time_since_epoch() / std::chrono::milliseconds(1);
}

struct info
{
    public:
        long long timeToProcess;
        int clientID;
        std::string msg;

        info(int _clientID, std::string _msg): 
                clientID(_clientID), msg(_msg)
        {
            timeToProcess = currentTime() + rand()%DELAY;
        }
};


template<typename T>
struct greaterThanByTime {
	bool operator()(const T &lhs, const T &rhs){
		return lhs.timeToProcess > rhs.timeToProcess;
	}
};


std::priority_queue<info, vector<info>, greaterThanByTime<info>> lat_msg;
std::priority_queue<info, vector<info>, greaterThanByTime<info>> lat_event;

