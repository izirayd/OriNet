#ifndef TypePacketProtocolH
#define TypePacketProtocolH

  // ���� ������������ ����������
  #define ORI 0
  #define ORI_TCP SOCK_STREAM  // 1
  #define ORI_UDP SOCK_DGRAM   // 2

 // ���������
  #define TYPE_STRUCT_PROTOCOL       0x01   // �������� ���������
  #define TYPE_LANGUAGE_PROTOCOL     0x02   // ������� ��������� ����������� ����� OriLng
  #define TYPE_FILE_PROTOCOL_READ    0x03   // �������� �����
  #define TYPE_FILE_PROTOCOL_SEND    0x04   // �������� �����
  #define TYPE_P2P_DOWNLOAD_PROTOCOL 0x05   // �������� �������� ������ � ������� ���������� ����
  #define TYPE_VOICE_PROTOCOL        0x06   // �������� �������� ��������� ���������
  #define TYPE_POINT_PROTOCOL        0x07   // ������ ��� �������� SendPacket � RecvPacket
  #define TYPE_SEND_SUPER_PACKETS    0x08   // �������� TSSP, ������ ��� ��������, ������� �������� �����, �������� �� ��������.
                                            // ���� � ���, ��� TLP, TSP, TPP - �� ����� �������� ������ ������ ~65 �� ��-�� ����������� � IPv4. TSSP �������� �� ������ ����
                                            // �������� ������, ����� ������������ ����������� �����, ��� �� ������ ������ �����.
  #define TYPE_RUN_ASYNC_SUPER_PACKET 0x09

#endif