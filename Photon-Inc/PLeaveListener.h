#pragma once
namespace PhotonLib {
	class PLeaveListener {
	public:
		/// <summary>
		/// �N�����ޏo�����Ƃ��̏���
		/// </summary>
		/// <param name="playerNr">�v���C���[�ԍ�</param>
		virtual void onLeave(int playerNr) = 0;
	};
}