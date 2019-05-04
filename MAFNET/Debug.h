#pragma once
#include <vector>
#include <stdarg.h>

namespace MAFNET {

	class DebugObserver;
	class Debug {
	public:
		enum MessageGravity {
			NONE = -1, DEBUG = 0, WARNING = 1, ALL = 2
		};
	private:
		static std::vector<DebugObserver*> observers;
		static MessageGravity currentMessageLevel;

		static void callObservers(Debug::MessageGravity messageGravity, std::string str);
	public:
		static void setMessageLevel(MessageGravity newLevel);

		static void assignObserver(DebugObserver* newObserver);

		static void removeObserver(DebugObserver* observer);

		static void writeMessage(Debug::MessageGravity messageGravity, const char* txt, ...);
	};

	class DebugObserver {
	public:
		virtual void onMessageStream(Debug::MessageGravity messageGravity, std::string debugMessage) = 0;
	};


}