#pragma once
#include <vector>
#include "LoadBalancing-cpp/inc/Client.h"
#include "PEventListener.h"

namespace PhotonLib {
	class PNetworkLogic : public ExitGames::LoadBalancing::Listener {
		//クラススコープで上手い事名前空間省略する方法がわからんのでusingの嵐。全然わからん!
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
		//photonのwebサイトで取得したappIDで初期化。appVersionは適当に決めていい。
		PNetworkLogic(const JString& appID, const JString& appVersion)
			:mLoadBalancingClient(*this, appID, appVersion, ExitGames::Photon::ConnectionProtocol::DEFAULT, true){};

		/*PEventListenerをpublic継承したクラスを登録できる。
		*登録したクラスのonPhotonEvent関数に他プレイヤーが投げたイベントが飛んでくる。*/
		void addEventListener(PEventListener* listener) {
			listenerVector.push_back(listener);
		}

		/*photonに接続。isConnecting()で繋がったか確認できる。
		接続中と接続試行中は何もしない。*/
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

		/*roomNameに指定した名前の部屋が無ければ、その名前とmaxPlayerに指定した最大人数で部屋を作り、入る。
		指定した名前の部屋が既にあった場合、ただ入るだけ。*/
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

		/*イベントを投げる。他プレイヤーに情報を伝えるということ。
		イベントの受け取り方はaddEventListener関数のコメントを見てね。
		sendReliable - 情報を確実に伝えるならtrue, 速度優先で情報が消えてしまっても気にしないならfalse。
		data -　伝えたいデータ。配列以外。
		eventCode - イベントの種別。自由に決めていい。nByte(unsigned char)型。
		*/
		template<typename Ftype>
		void raiseEvent(bool sendReliable, const Ftype &data, nByte eventCode, const RaiseEventOptions& options = RaiseEventOptions()) {
			if (!mLoadBalancingClient.opRaiseEvent(sendReliable, data, eventCode, options)) {
				EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"Could not raise event.");
			}
		}

		//raiseEventの配列用。dataには配列の先頭ポインタを、sizeには要素数を入れる。
		template<typename Ftype>
		void raiseEvent(bool sendReliable, const Ftype* data, int size, nByte eventCode, const RaiseEventOptions& options = RaiseEventOptions()) {
			if (!mLoadBalancingClient.opRaiseEvent(sendReliable, data, size, eventCode, options)) {
				EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"Could not raise event.");
			}
		}

		//全てのイベントリスナーを登録解除する
		void removeAllListener() {
			listenerVector.clear();
		}

		//引数に渡したイベントリスナーを登録解除する。
		void removeListener(PEventListener* listener);

		//ゲームループ内で呼び出し続けてください
		void Update() {
			mLoadBalancingClient.service();
		}

	//接続状態enum
		enum ConnectState {
			DISCONNECT,//切断
			TRY_CONNECT,//接続試行中
			CONNECT,//接続完了
			TRY_ROOMIN,//ルームイン試行中
			ROOMIN//ルームイン完了
		};
	private:
	/*************メンバ変数(private)**************/
		ExitGames::LoadBalancing::Client mLoadBalancingClient;
		ExitGames::Common::Logger mLogger; // accessed by EGLOG()

		std::vector<PEventListener*> listenerVector;

		ConnectState state;
		int errorCode = 0;
		int warningCode = 0;
		int localPlayerNum = -1;
		JString errorMessage;

	/*************リスナー関数(自分で呼んだりしない)************/
		//接続しているか、ルームにいるかなどの情報が変更された場合の重複した処理
		bool stateChange(ConnectState newState, int errorCode, const JString& errStr);

		// デバッグを受信して印刷する
		void debugReturn(int debugLevel, const JString& string) override {
			EGLOG(debugLevel, string);
		}

		//ここにエラー処理を実装する
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

		//同じ部屋にいる全プレイヤーの特定の操作によって引き起こされるイベント
		void joinRoomEventAction(int playerNr, const JVector<int>& playernrs, const Player& player) override {};
		void leaveRoomEventAction(int playerNr, bool isInactive) override {};

		void customEventAction(int playerNr, nByte eventCode, const Object& eventContent) override;

		//サーバー上の操作のためのコールバック
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

		//情報、特定の値が更新されたとき
		void onRoomListUpdate(void) override {}
		void onRoomPropertiesChange(const Hashtable& /*changes*/) override {}
		void onPlayerPropertiesChange(int /*playerNr*/, const Hashtable& /*changes*/) override {}
		void onAppStatsUpdate(void) override {}
		void onLobbyStatsUpdate(const JVector<LobbyStatsResponse>& /*lobbyStats*/) override {}
		void onCacheSliceChanged(int /*cacheSliceIndex*/) override {}
		void onMasterClientChanged(int /*id*/, int /*oldID*/) override {};

		//カスタム認証
		void onCustomAuthenticationIntermediateStep(const Dictionary<JString, Object>& /*customResponseData*/) override {}

		// 接続ワークフロー中に利用可能なサーバーリージョンを受け取る（コンストラクターでリージョンを選択したいと指定した場合）
		void onAvailableRegions(const JVector<JString>& /*availableRegions*/, const JVector<JString>& /*availableRegionServers*/) override {}

		void onSecretReceival(const JString& /*secret*/) override {}

		void onDirectMessage(const Object& /*msg*/, int /*remoteID*/, bool /*relay*/) override {};

		void onCustomOperationResponse(const OperationResponse& operationResponse) override {};
	};
}