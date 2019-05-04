#include "Debug.h"

namespace MAFNET {
	std::vector<DebugObserver*> Debug::observers;
	Debug::MessageGravity Debug::currentMessageLevel = Debug::MessageGravity::WARNING;

	void Debug::callObservers(MessageGravity messageGravity, std::string str) {
		for (auto I : observers)
			I->onMessageStream(messageGravity, str);
	}

	void Debug::setMessageLevel(MessageGravity newLevel) {
		currentMessageLevel = newLevel;
	}

	void Debug::assignObserver(DebugObserver * newObserver) {
		observers.push_back(newObserver);
	}

	void Debug::removeObserver(DebugObserver * observer) {
		for (auto I = observers.begin(); I != observers.end(); I++) {
			if ((*I) == observer) {
				observers.erase(I);
				return;
			}
		}
		//the observer doesn't exist.
	}

	void Debug::writeMessage(Debug::MessageGravity messageGravity, const char * txt, ...) {
		if (currentMessageLevel < messageGravity)
			return;
		
		va_list valist;

		char buffer[256];
		memset(buffer, 0, sizeof(char) * 256);

		va_start(valist, txt);
		vsprintf_s(buffer, txt, valist);
		va_end(valist);

		callObservers(messageGravity, std::string(buffer));
	}
}