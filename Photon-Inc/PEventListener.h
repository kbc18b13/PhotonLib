#pragma once
#include "Common-cpp/inc/Object.h"

namespace PhotonLib {
	class PEventListener {
	public:
		virtual void onPhotonEvent
		(int playerNr, nByte eventCode, const ExitGames::Common::Object& eventContent) = 0;
	};
}