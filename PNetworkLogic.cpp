#include "PNetworkLogic.h"

namespace PhotonLib {
	void PNetworkLogic::removeListener(PEventListener * listener) {
		for (auto itr = eventVector.begin(); itr != eventVector.end(); itr++) {
			if ((*itr) == listener) {
				eventVector.erase(itr);
				break;
			}
		}
	}

	void PNetworkLogic::removeListener(PLeaveListener * listener) {
		for (auto itr = leaveVector.begin(); itr != leaveVector.end(); itr++) {
			if ((*itr) == listener) {
				leaveVector.erase(itr);
				break;
			}
		}
	}

	bool PNetworkLogic::stateChange(ConnectState newState, int errorCode, const JString & errStr) {
		if (errorCode == 0) {
			state = newState;
			return true;
		} else {
			this->errorCode = errorCode;
			errorMessage = errStr;
			return false;
		}
	}

	void PNetworkLogic::joinRoomEventAction(int playerNr, const JVector<int>& playernrs, const Player & player) {
		if (playersNum.empty()) {
			for (int i = 0; i < playernrs.getSize(); i++) {
				playersNum.insert(playernrs[i]);
			}
		} else {
			playersNum.insert(playerNr);
		}
	};

	void PNetworkLogic::leaveRoomEventAction(int playerNr, bool isInactive) {
		playersNum.erase(playerNr);
		for (PLeaveListener* leLis : leaveVector) {
			leLis->onLeave(playerNr);
		}
	}

	void PNetworkLogic::customEventAction(int playerNr, nByte eventCode, const Object & eventContent) {
		for (PEventListener* evLis : eventVector) {
			evLis->onPhotonEvent(playerNr, eventCode, eventContent);
		}
	}
}