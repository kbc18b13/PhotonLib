#include "PNetworkLogic.h"

namespace PhotonLib {
	void PNetworkLogic::removeListener(PEventListener * listener) {
		for (auto itr = listenerVector.begin(); itr != listenerVector.end(); itr++) {
			if ((*itr) == listener) {
				listenerVector.erase(itr);
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

	void PNetworkLogic::customEventAction(int playerNr, nByte eventCode, const Object & eventContent) {
		for (PEventListener* evLis : listenerVector) {
			evLis->onPhotonEvent(playerNr, eventCode, eventContent);
		}
	}
}