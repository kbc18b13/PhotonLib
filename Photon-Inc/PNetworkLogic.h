#pragma once
#include <vector>
#include "LoadBalancing-cpp/inc/Client.h"
#include "PEventListener.h"

namespace PhotonLib {
	class PNetworkLogic : public ExitGames::LoadBalancing::Listener {
		//�N���X�X�R�[�v�ŏ�肢�����O��ԏȗ�������@���킩���̂�using�̗��B�S�R�킩���!
		template <typename T, typename U>
		using Dictionary = ExitGames::Common::Dictionary<T, U>;
		using Hashtable = ExitGames::Common::Hashtable;
		using JString = ExitGames::Common::JString;
		template <typename T>
		using JVector = ExitGames::Common::JVector<T>;
		using LobbyStatsResponse = ExitGames::LoadBalancing::LobbyStatsResponse;
		using Object = ExitGames::Common::Object;
		using OperationResponse = ExitGames::Photon::OperationResponse;
		using Player = ExitGames::LoadBalancing::Player;
		using RaiseEventOptions = ExitGames::LoadBalancing::RaiseEventOptions;

	public:
		//photon��web�T�C�g�Ŏ擾����appID�ŏ������BappVersion�͓K���Ɍ��߂Ă����B
		PNetworkLogic(const JString& appID, const JString& appVersion)
			:mLoadBalancingClient(*this, appID, appVersion, ExitGames::Photon::ConnectionProtocol::DEFAULT, true){};

		/*PEventListener��public�p�������N���X��o�^�ł���B
		*�o�^�����N���X��onPhotonEvent�֐��ɑ��v���C���[���������C�x���g�����ł���B*/
		void addEventListener(PEventListener* listener) {
			listenerVector.push_back(listener);
		}

		/*photon�ɐڑ��BisConnecting()�Ōq���������m�F�ł���B
		�ڑ����Ɛڑ����s���͉������Ȃ��B*/
		void connect() {
			if (state == DISCONNECT) {
				if (mLoadBalancingClient.connect()) {
					EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"Could not connect.");
				}
				state = TRY_CONNECT;
			}
		}

		void disconnect() {
			mLoadBalancingClient.disconnect();
		}

		bool getErrorCode() {
			return errorCode;
		}

		JString getErrorMessage() {
			return errorMessage;
		}

		int getLocalPlayerNum() {
			return localPlayerNum;
		}

		bool isConnecting() {
			return state >= CONNECT;
		}

		bool isError() {
			return errorCode != 0;
		}

		bool isRoomIn() {
			return state == ROOMIN;
		}

		/*roomName�Ɏw�肵�����O�̕�����������΁A���̖��O��maxPlayer�Ɏw�肵���ő�l���ŕ��������A����B
		�w�肵�����O�̕��������ɂ������ꍇ�A�������邾���B*/
		void joinOrCreateRoom(ExitGames::Common::JString roomName, nByte maxPlayer) {
			if (state == CONNECT) {
				ExitGames::LoadBalancing::RoomOptions option;
				option.setMaxPlayers(maxPlayer);
				if (mLoadBalancingClient.opJoinOrCreateRoom(roomName, option)) {
					EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"Could not join or createRoom.");
				}
				state = TRY_ROOMIN;
			}
		}

		/*�C�x���g�𓊂���B���v���C���[�ɏ���`����Ƃ������ƁB
		�C�x���g�̎󂯎�����addEventListener�֐��̃R�����g�����ĂˁB
		sendReliable - �����m���ɓ`����Ȃ�true, ���x�D��ŏ�񂪏����Ă��܂��Ă��C�ɂ��Ȃ��Ȃ�false�B
		data -�@�`�������f�[�^�B�z��ȊO�B
		eventCode - �C�x���g�̎�ʁB���R�Ɍ��߂Ă����BnByte(unsigned char)�^�B
		*/
		template<typename Ftype>
		void raiseEvent(bool sendReliable, const Ftype &data, nByte eventCode, const RaiseEventOptions& options = RaiseEventOptions()) {
			if (!mLoadBalancingClient.opRaiseEvent(sendReliable, data, eventCode, options)) {
				EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"Could not raise event.");
			}
		}

		//raiseEvent�̔z��p�Bdata�ɂ͔z��̐擪�|�C���^���Asize�ɂ͗v�f��������B
		template<typename Ftype>
		void raiseEvent(bool sendReliable, const Ftype* data, int size, nByte eventCode, const RaiseEventOptions& options = RaiseEventOptions()) {
			if (!mLoadBalancingClient.opRaiseEvent(sendReliable, data, size, eventCode, options)) {
				EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"Could not raise event.");
			}
		}

		//�S�ẴC�x���g���X�i�[��o�^��������
		void removeAllListener() {
			listenerVector.clear();
		}

		//�����ɓn�����C�x���g���X�i�[��o�^��������B
		void removeListener(PEventListener* listener);

		//�Q�[�����[�v���ŌĂяo�������Ă�������
		void Update() {
			mLoadBalancingClient.service();
		}

	//�ڑ����enum
		enum ConnectState {
			DISCONNECT,//�ؒf
			TRY_CONNECT,//�ڑ����s��
			CONNECT,//�ڑ�����
			TRY_ROOMIN,//���[���C�����s��
			ROOMIN//���[���C������
		};
	private:
	/*************�����o�ϐ�(private)**************/
		ExitGames::LoadBalancing::Client mLoadBalancingClient;
		ExitGames::Common::Logger mLogger; // accessed by EGLOG()

		std::vector<PEventListener*> listenerVector;

		ConnectState state;
		int errorCode = 0;
		int warningCode = 0;
		int localPlayerNum = -1;
		JString errorMessage;

	/*************���X�i�[�֐�(�����ŌĂ񂾂肵�Ȃ�)************/
		//�ڑ����Ă��邩�A���[���ɂ��邩�Ȃǂ̏�񂪕ύX���ꂽ�ꍇ�̏d����������
		bool stateChange(ConnectState newState, int errorCode, const JString& errStr);

		// �f�o�b�O����M���Ĉ������
		void debugReturn(int debugLevel, const JString& string) override {
			EGLOG(debugLevel, string);
		}

		//�����ɃG���[��������������
		void connectionErrorReturn(int errorCode) override {
			this->errorCode = errorCode;
			errorMessage = L"connection error";
		}
		void clientErrorReturn(int errorCode) override {
			this->errorCode = errorCode;
			errorMessage = L"client error";
		}
		void warningReturn(int warningCode) override {
			this->warningCode = errorCode;
			errorMessage = L"warning but not error";
		}
		void serverErrorReturn(int errorCode) override {
			this->errorCode = errorCode;
			errorMessage = L"server error";
		}

		//���������ɂ���S�v���C���[�̓���̑���ɂ���Ĉ����N�������C�x���g
		void joinRoomEventAction(int playerNr, const JVector<int>& playernrs, const Player& player) override {};
		void leaveRoomEventAction(int playerNr, bool isInactive) override {};

		void customEventAction(int playerNr, nByte eventCode, const Object& eventContent) override;

		//�T�[�o�[��̑���̂��߂̃R�[���o�b�N
		void connectReturn(int errorCode, const JString& errorString, const JString& region, const JString& cluster) override {
			stateChange(CONNECT, errorCode, errorString);
		}

		void disconnectReturn(void) override {
			state = DISCONNECT;
			localPlayerNum = -1;
		}

		void createRoomReturn(int localPlayerNr, const Hashtable& roomProperties, const Hashtable& playerProperties, int errorCode, const JString& errorString) override {
			if (stateChange(ROOMIN, errorCode, errorString)) { localPlayerNum = localPlayerNr; }
		}

		void joinOrCreateRoomReturn(int localPlayerNr, const Hashtable& roomProperties, const Hashtable& playerProperties, int errorCode, const JString& errorString) override {
			if (stateChange(ROOMIN, errorCode, errorString)) { localPlayerNum = localPlayerNr; }
		}

		void joinRoomReturn(int localPlayerNr, const Hashtable& roomProperties, const Hashtable& playerProperties, int errorCode, const JString& errorString) override {
			if (stateChange(ROOMIN, errorCode, errorString)) { localPlayerNum = localPlayerNr; }
		}

		void joinRandomRoomReturn(int localPlayerNr, const Hashtable& roomProperties, const Hashtable& playerProperties, int errorCode, const JString& errorString) override {
			if (stateChange(ROOMIN, errorCode, errorString)) { localPlayerNum = localPlayerNr; }
		}

		void leaveRoomReturn(int errorCode, const JString& errorString) override {
			stateChange(CONNECT, errorCode, errorString);
			localPlayerNum = -1;
		}

		void joinLobbyReturn(void) override {};
		void leaveLobbyReturn(void) override {};
		void onFindFriendsResponse(void) override {}
		void onLobbyStatsResponse(const JVector<LobbyStatsResponse>& /*lobbyStats*/) override {}
		void webRpcReturn(int /*errorCode*/, const JString& /*errorString*/, const JString& /*uriPath*/, int /*resultCode*/, const Dictionary<Object, Object>& /*returnData*/) override {}

		//���A����̒l���X�V���ꂽ�Ƃ�
		void onRoomListUpdate(void) override {}
		void onRoomPropertiesChange(const Hashtable& /*changes*/) override {}
		void onPlayerPropertiesChange(int /*playerNr*/, const Hashtable& /*changes*/) override {}
		void onAppStatsUpdate(void) override {}
		void onLobbyStatsUpdate(const JVector<LobbyStatsResponse>& /*lobbyStats*/) override {}
		void onCacheSliceChanged(int /*cacheSliceIndex*/) override {}
		void onMasterClientChanged(int /*id*/, int /*oldID*/) override {};

		//�J�X�^���F��
		void onCustomAuthenticationIntermediateStep(const Dictionary<JString, Object>& /*customResponseData*/) override {}

		// �ڑ����[�N�t���[���ɗ��p�\�ȃT�[�o�[���[�W�������󂯎��i�R���X�g���N�^�[�Ń��[�W������I���������Ǝw�肵���ꍇ�j
		void onAvailableRegions(const JVector<JString>& /*availableRegions*/, const JVector<JString>& /*availableRegionServers*/) override {}

		void onSecretReceival(const JString& /*secret*/) override {}

		void onDirectMessage(const Object& /*msg*/, int /*remoteID*/, bool /*relay*/) override {};

		void onCustomOperationResponse(const OperationResponse& operationResponse) override {};
	};
}