#include <iostream>
#include <cstdio>
#include <chrono>
#include <mutex>
#include <queue>
#include <thread>
#include <condition_variable>

using namespace std;

std::mutex mu;
std::condition_variable condvar;
std::queue<int> msgQueue;

void provider(int start, int end)
{
	
	for(int i = start; i <= end; i++)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(200));
		{
			std::lock_guard<std::mutex> guard(mu);
			msgQueue.push(i);

		}
		printf("create msg %d.\n", i);
		condvar.notify_all();
	}

}

void consumer(int demand)
{
	while(demand > 0)
	{
		std::unique_lock<std::mutex> lock(mu);
		condvar.wait(lock, []{return msgQueue.size() > 0;});
		printf("consumer msg %d\n", msgQueue.front());
		msgQueue.pop();
		demand--;
	}
}

struct sthread
{
	explicit sthread(std::thread&& tt): th_(std::move(tt))
	{
		if(!th_.joinable())
		{
			throw std::logic_error("No thread!");	
		}
	}
	~sthread()
	{
		if(th_.joinable())
		{
			//th_.join();
			th_.detach();
		}
	}
private:
	sthread(sthread const &) = delete;
	sthread& operator=(sthread const& ) = delete;
	std::thread th_;
};

int main()
{
/*	std::thread pp1(provider, 0, 9);
	std::thread pp2(provider, 10, 19);
	std::thread pp3(provider, 20, 29);
	std::thread pc1(consumer, 20);
	std::thread pc2(consumer, 10);
*/
	sthread spp1(std::thread(provider, 0, 9));
	sthread spp2(std::thread(provider, 10, 19));
	sthread spp3(std::thread(provider, 20, 29));
	sthread spc1(std::thread(consumer, 20));
	sthread spc2(std::thread(consumer, 10));

	for(int i = 0; i < 1; i++ )
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));
		printf("have passed [%d] second.\n", i+1);
	}
/*	pp1.join();
	pp2.join();
	pp3.join();
	pc1.join();
	pc2.join();*/
	return 0;
}
