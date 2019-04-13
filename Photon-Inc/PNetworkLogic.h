#pragma once
#include <vector>
#include "LoadBalancing-cpp/inc/Client.h"
#include "PEventListener.h"

namespace PhotonLib {
	class PNetworkLogic : public ExitGames::LoadBalancing::Listener {
		//クラススコープで名前空間省略。冗長。
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
		/// 初期化。
		/// </summary>
		/// <param name="appID">photonのサイトで取得したappID</param>
		/// <param name="appVersion">バージョン。適当に決めていい。</param>
		PNetworkLogic(const JString& appID, const JString& appVersion)
			:mLoadBalancingClient(*this, appID, appVersion, ExitGames::Photon::ConnectionProtocol::DEFAULT, true){};

		/// <summary>
		/// PEventListenerを継承したクラスを登録する
		/// </summary>
		/// <remarks>
		/// 登録したクラスのonPhotonEvent関数に他プレイヤーが投げたイベントが飛んでくる。
		/// </remarks>
		/// <param name="listener"></param>
		void addEventListener(PEventListener* listener) {
			listenerVector.push_back(listener);
		}

		/// <summary>
		/// photonに接続。接続中と接続試行中は何もしない。
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
		/// photonから切断。
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
		/// 自分のいる部屋の参照を取得。
		/// </summary>
		/// <remarks>その参照が取得された部屋を出た後に参照されたインスタンスにアクセスするときの振る舞い、
		/// および部屋の中にいないでこの関数を呼び出すときの振る舞いは未定義です。コピーへの操作は影響しません。</remarks>
		MutableRoom getJoinedRoom() {
			return mLoadBalancingClient.getCurrentlyJoinedRoom();
		}

		/// <summary>
		/// 自分のプレイヤーの参照を取得。
		/// </summary>
		MutablePlayer getLocalPlayer() {
			return mLoadBalancingClient.getLocalPlayer();
		}

		/// <summary>
		/// 自分のプレイヤー番号を取得。
		/// </summary>
		int getLocalPlayerNum() {
			return mLoadBalancingClient.getLocalPlayer().getNumber();
		}

		/// <summary>
		/// 部屋の一覧を取得。部屋に居ると最新の情報は入手できません。
		/// </summary>
		const JVector<Room*>& getRoomList() {
			return mLoadBalancingClient.getRoomList();
		}

		/// <summary>
		/// photon接続状態にあるならtrue。
		/// </summary>
		bool isConnecting() {
			return state >= CONNECT;
		}

		/// <summary>
		/// エラーコードが0以外のときtrue。
		/// </summary>
		bool isError() {
			return errorCode != 0;
		}

		/// <summary>
		/// 部屋に入っていればtrue。
		/// </summary>
		bool isRoomIn() {
			return state == ROOMIN;
		}

		/// <summary>
		/// 部屋が既にあれば入室する。無ければ作り、入室する。
		/// </summary>
		/// <param name="roomName">部屋の名前。</param>
		/// <param name="maxPlayer">最大プレイヤー数</param>
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
		/// 部屋を出ます。
		/// </summary>
		void leaveRoom() {
			if (state == ROOMIN) {
				mLoadBalancingClient.opLeaveRoom();
			}
		}

		template<typename Ftype>
		/// <summary>
		///  データを他プレイヤーに投げる。文字型の配列はJString型として扱われるためこっち。
		/// </summary>
		/// <param name="sendReliable">確実に届けるならtrue。途中で消えてもいいならfalse。</param>
		/// <param name="data">届けるデータ。</param>
		/// <param name="eventCode">イベントの種別。自由に決めていい。</param>
		/// <param name="options">様々なオプションを指定できる。</param>
		void raiseEvent(bool sendReliable, const Ftype &data, nByte eventCode, const RaiseEventOptions& options = RaiseEventOptions()) {
			if (!mLoadBalancingClient.opRaiseEvent(sendReliable, data, eventCode, options)) {
				EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"Could not raise event.");
			}
		}

		template<typename Ftype>
		/// <summary>
		/// 配列データを他プレイヤーに投げる。文字型の配列はこっちでは無い。
		/// </summary>
		/// <param name="sendReliable">確実に届けるならtrue。途中で消えてもいいならfalse。</param>
		/// <param name="data">届けるデータ。</param>
		/// <param name="eventCode">イベントの種別。自由に決めていい。</param>
		/// <param name="options">様々なオプションを指定できる。</param>
		void raiseEvent(bool sendReliable, const Ftype* data, int size, nByte eventCode, const RaiseEventOptions& options = RaiseEventOptions()) {
			if (!mLoadBalancingClient.opRaiseEvent(sendReliable, data, size, eventCode, options)) {
				EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"Could not raise event.");
			}
		}

		/// <summary>
		/// 全てのイベントリスナーを登録解除する
		/// </summary>
		void removeAllListener() {
			listenerVector.clear();
		}

		/// <summary>
		/// イベントリスナーを登録解除する。
		/// </summary>
		/// <param name="listener">登録解除したいリスナー</param>
		void removeListener(PEventListener* listener);

		/// <summary>
		/// ゲームループ内で呼び出し続けてください
		/// </summary>
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
		ExitGames::LoadBalancing::Client mLoadBalancingClient; //photonへの操作を行うクライアント
		ExitGames::Common::Logger mLogger; // accessed by EGLOG()

		std::vector<PEventListener*> listenerVector; //イベントリスナー入れ

		ConnectState state = DISCONNECT; //接続状態
		int errorCode = 0; //直前の操作のエラーコード
		int warningCode = 0;
		JString errorMessage;

	/*************ここから下の関数を外部から使うことはありません。************/
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