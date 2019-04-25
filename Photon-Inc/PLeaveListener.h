#pragma once
namespace PhotonLib {
	class PLeaveListener {
	public:
		/// <summary>
		/// 誰かが退出したときの処理
		/// </summary>
		/// <param name="playerNr">プレイヤー番号</param>
		virtual void onLeave(int playerNr) = 0;
	};
}