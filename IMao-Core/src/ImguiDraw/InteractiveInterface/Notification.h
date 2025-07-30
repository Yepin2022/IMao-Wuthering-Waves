#pragma once
#include <vector>
#include <string>
#include <thread>
class NotificationDatas 
{
public:
	std::string content;
	int timeDuration = 3;

	NotificationDatas(std::string content, int timeDuration) : content(content),timeDuration(timeDuration) {}
};

class Notification
{
public:
	static void Start() {
		timerStopFlag = false;
		timerThread = std::thread(&Notification::Timer);
	}

	static void Stop() {
		timerStopFlag = true;
		timerThread.join();
	}
    static void DrawInfo();
	static void AddInfo(NotificationDatas addNotificationDatas);

private:
	static void Timer();
	static std::vector<NotificationDatas> notifications;
	static std::thread timerThread;
	static bool timerStopFlag;
};

