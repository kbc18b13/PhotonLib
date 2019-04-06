#pragma once
#include "Common-cpp/inc/Object.h"

namespace PhotonLib {
	class PEventListener {
	public:
		/// <summary>
		/// <para>他プレイヤーが投げたイベントを受けとる。</para>
		/// <para>使用するにはPNetworkLogicにインスタンスを登録してください。</para>
		/// </summary>
		/// <remarks>
		/// <para>Object型から元の型に戻すには</para>
		/// <para>ValueObject<元の型名>(eventContent).getDataCopy() でコピーを取得</para>
		/// <para>ValueObject<元の型名>(eventContent).getDataAddress() でアドレスを取得</para>
		/// <para>配列の場合、getDataCopy()を使っても配列そのものはコピーされないので別途コピーが必要です。</para>
		/// </remarks>
		/// <param name="playerNr">投げたプレイヤーの番号</param>
		/// <param name="eventCode">イベントの種別</param>
		/// <param name="eventContent">届いたデータ</param>
		virtual void onPhotonEvent
		(int playerNr, nByte eventCode, const ExitGames::Common::Object& eventContent) = 0;
	};
}