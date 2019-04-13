#pragma once
#include <vector>
#include "LoadBalancing-cpp/inc/Client.h"
#include "PEventListener.h"

namespace PhotonLib {
	class PNetworkLogic : public ExitGames::LoadBalancing::Listener {
		//�N���X�X�R�[�v�Ŗ��O��ԏȗ��B�璷�B
		template <typename T, typename U>
		using Dictionary = ExitGames::Common::Dictionary<T, U>;
		using Hashtable = ExitGames::Common::Hashtable;
		using JString = ExitGames::Common::JString;
		template <typename T>
		using JVector = ExitGames::Common::JVector<T>;
		using LobbyStatsResponse = ExitGames::LoadBalancing::LobbyStatsResponse;
		using MutablePlayer = ExitGames::LoadBalancing::MutablePlayer;
		using MutableRoom = ExitGames::LoadBalancing::MutableRoom;
		using Object = ExitGames::Common::Object;
		using OperationResponse = ExitGames::Photon::OperationResponse;
		using Player = ExitGames::LoadBalancing::Player;
		using RaiseEventOptions = ExitGames::LoadBalancing::RaiseEventOptions;
		using Room = ExitGames::LoadBalancing::Room;

	public:
		/// <summary>
		/// �������B
		/// </summary>
		/// <param name="appID">photon�̃T�C�g�Ŏ擾����appID</param>
		/// <param name="appVersion">�o�[�W�����B�K���Ɍ��߂Ă����B</param>
		PNetworkLogic(const JString& appID, const JString& appVersion)
			:mLoadBalancingClient(*this, appID, appVersion, ExitGames::Photon::ConnectionProtocol::DEFAULT, true){};

		/// <summary>
		/// PEventListener���p�������N���X��o�^����
		/// </summary>
		/// <remarks>
		/// �o�^�����N���X��onPhotonEvent�֐��ɑ��v���C���[���������C�x���g�����ł���B
		/// </remarks>
		/// <param name="listener"></param>
		void addEventListener(PEventListener* listener) {
			listenerVector.push_back(listener);
		}

		/// <summary>
		/// photon�ɐڑ��B�ڑ����Ɛڑ����s���͉������Ȃ��B
		/// </summary>
		void connect() {
			if (state == DISCONNECT) {
				if (mLoadBalancingClient.connect()) {
					EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"Could not connect.");
				}
				state = TRY_CONNECT;
			}
		}

		/// <summary>
		/// photon����ؒf�B
		/// </summary>
		void disconnect() {
			mLoadBalancingClient.disconnect();
		}

		bool getErrorCode() {
			return errorCode;
		}

		JString getErrorMessage() {
			return errorMessage;
		}

		/// <summary>
		/// �����̂��镔���̎Q�Ƃ��擾�B
		/// </summary>
		/// <remarks>���̎Q�Ƃ��擾���ꂽ�������o����ɎQ�Ƃ��ꂽ�C���X�^���X�ɃA�N�Z�X����Ƃ��̐U�镑���A
		/// ����ѕ����̒��ɂ��Ȃ��ł��̊֐����Ăяo���Ƃ��̐U�镑���͖���`�ł��B�R�s�[�ւ̑���͉e�����܂���B</remarks>
		MutableRoom getJoinedRoom() {
			return mLoadBalancingClient.getCurrentlyJoinedRoom();
		}

		/// <summary>
		/// �����̃v���C���[�̎Q�Ƃ��擾�B
		/// </summary>
		MutablePlayer getLocalPlayer() {
			return mLoadBalancingClient.getLocalPlayer();
		}

		/// <summary>
		/// �����̃v���C���[�ԍ����擾�B
		/// </summary>
		int getLocalPlayerNum() {
			return mLoadBalancingClient.getLocalPlayer().getNumber();
		}

		/// <summary>
		/// �����̈ꗗ���擾�B�����ɋ���ƍŐV�̏��͓���ł��܂���B
		/// </summary>
		const JVector<Room*>& getRoomList() {
			return mLoadBalancingClient.getRoomList();
		}

		/// <summary>
		/// photon�ڑ���Ԃɂ���Ȃ�true�B
		/// </summary>
		bool isConnecting() {
			return state >= CONNECT;
		}

		/// <summary>
		/// �G���[�R�[�h��0�ȊO�̂Ƃ�true�B
		/// </summary>
		bool isError() {
			return errorCode != 0;
		}

		/// <summary>
		/// �����ɓ����Ă����true�B
		/// </summary>
		bool isRoomIn() {
			return state == ROOMIN;
		}

		/// <summary>
		/// ���������ɂ���Γ�������B������΍��A��������B
		/// </summary>
		/// <param name="roomName">�����̖��O�B</param>
		/// <param name="maxPlayer">�ő�v���C���[��</param>
		void joinOrCreateRoom(JString roomName, nByte maxPlayer) {
			if (state == CONNECT) {
				ExitGames::LoadBalancing::RoomOptions option;
				option.setMaxPlayers(maxPlayer);
				if (mLoadBalancingClient.opJoinOrCreateRoom(roomName, option)) {
					EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"Could not join or createRoom.");
				}
				state = TRY_ROOMIN;
			}
		}

		/// <summary>
		/// �������o�܂��B
		/// </summary>
		void leaveRoom() {
			if (state == ROOMIN) {
				mLoadBalancingClient.opLeaveRoom();
			}
		}

		template<typename Ftype>
		/// <summary>
		///  �f�[�^�𑼃v���C���[�ɓ�����B�����^�̔z���JString�^�Ƃ��Ĉ����邽�߂������B
		/// </summary>
		/// <param name="sendReliable">�m���ɓ͂���Ȃ�true�B�r���ŏ����Ă������Ȃ�false�B</param>
		/// <param name="data">�͂���f�[�^�B</param>
		/// <param name="eventCode">�C�x���g�̎�ʁB���R�Ɍ��߂Ă����B</param>
		/// <param name="options">�l�X�ȃI�v�V�������w��ł���B</param>
		void raiseEvent(bool sendReliable, const Ftype &data, nByte eventCode, const RaiseEventOptions& options = RaiseEventOptions()) {
			if (!mLoadBalancingClient.opRaiseEvent(sendReliable, data, eventCode, options)) {
				EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"Could not raise event.");
			}
		}

		template<typename Ftype>
		/// <summary>
		/// �z��f�[�^�𑼃v���C���[�ɓ�����B�����^�̔z��͂������ł͖����B
		/// </summary>
		/// <param name="sendReliable">�m���ɓ͂���Ȃ�true�B�r���ŏ����Ă������Ȃ�false�B</param>
		/// <param name="data">�͂���f�[�^�B</param>
		/// <param name="eventCode">�C�x���g�̎�ʁB���R�Ɍ��߂Ă����B</param>
		/// <param name="options">�l�X�ȃI�v�V�������w��ł���B</param>
		void raiseEvent(bool sendReliable, const Ftype* data, int size, nByte eventCode, const RaiseEventOptions& options = RaiseEventOptions()) {
			if (!mLoadBalancingClient.opRaiseEvent(sendReliable, data, size, eventCode, options)) {
				EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"Could not raise event.");
			}
		}

		/// <summary>
		/// �S�ẴC�x���g���X�i�[��o�^��������
		/// </summary>
		void removeAllListener() {
			listenerVector.clear();
		}

		/// <summary>
		/// �C�x���g���X�i�[��o�^��������B
		/// </summary>
		/// <param name="listener">�o�^�������������X�i�[</param>
		void removeListener(PEventListener* listener);

		/// <summary>
		/// �Q�[�����[�v���ŌĂяo�������Ă�������
		/// </summary>
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
		ExitGames::LoadBalancing::Client mLoadBalancingClient; //photon�ւ̑�����s���N���C�A���g
		ExitGames::Common::Logger mLogger; // accessed by EGLOG()

		std::vector<PEventListener*> listenerVector; //�C�x���g���X�i�[����

		ConnectState state = DISCONNECT; //�ڑ����
		int errorCode = 0; //���O�̑���̃G���[�R�[�h
		int warningCode = 0;
		JString errorMessage;

	/*************�������牺�̊֐����O������g�����Ƃ͂���܂���B************/
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
		}

		void createRoomReturn(int localPlayerNr, const Hashtable& roomProperties, const Hashtable& playerProperties, int errorCode, const JString& errorString) override {
			stateChange(ROOMIN, errorCode, errorString);
		}

		void joinOrCreateRoomReturn(int localPlayerNr, const Hashtable& roomProperties, const Hashtable& playerProperties, int errorCode, const JString& errorString) override {
			stateChange(ROOMIN, errorCode, errorString);
		}

		void joinRoomReturn(int localPlayerNr, const Hashtable& roomProperties, const Hashtable& playerProperties, int errorCode, const JString& errorString) override {
			stateChange(ROOMIN, errorCode, errorString);
		}

		void joinRandomRoomReturn(int localPlayerNr, const Hashtable& roomProperties, const Hashtable& playerProperties, int errorCode, const JString& errorString) override {
			stateChange(ROOMIN, errorCode, errorString);
		}

		void leaveRoomReturn(int errorCode, const JString& errorString) override {
			stateChange(CONNECT, errorCode, errorString);
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