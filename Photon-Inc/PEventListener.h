#pragma once
#include "Common-cpp/inc/Object.h"

namespace PhotonLib {
	class PEventListener {
	public:
		/// <summary>
		/// <para>���v���C���[���������C�x���g���󂯂Ƃ�B</para>
		/// <para>�g�p����ɂ�PNetworkLogic�ɃC���X�^���X��o�^���Ă��������B</para>
		/// </summary>
		/// <remarks>
		/// <para>Object�^���猳�̌^�ɖ߂��ɂ�</para>
		/// <para>ValueObject<���̌^��>(eventContent).getDataCopy() �ŃR�s�[���擾</para>
		/// <para>ValueObject<���̌^��>(eventContent).getDataAddress() �ŃA�h���X���擾</para>
		/// <para>�z��̏ꍇ�AgetDataCopy()���g���Ă��z�񂻂̂��̂̓R�s�[����Ȃ��̂ŕʓr�R�s�[���K�v�ł��B</para>
		/// </remarks>
		/// <param name="playerNr">�������v���C���[�̔ԍ�</param>
		/// <param name="eventCode">�C�x���g�̎��</param>
		/// <param name="eventContent">�͂����f�[�^</param>
		virtual void onPhotonEvent
		(int playerNr, nByte eventCode, const ExitGames::Common::Object& eventContent) = 0;
	};
}