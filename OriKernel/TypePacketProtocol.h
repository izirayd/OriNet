#ifndef TypePacketProtocolH
#define TypePacketProtocolH

  // Типы транспортных протоколов
  #define ORI 0
  #define ORI_TCP SOCK_STREAM  // 1
  #define ORI_UDP SOCK_DGRAM   // 2

 // Протоколы
  #define TYPE_STRUCT_PROTOCOL       0x01   // Байтовые структуры
  #define TYPE_LANGUAGE_PROTOCOL     0x02   // Плоские структуры скриптового языка OriLng
  #define TYPE_FILE_PROTOCOL_READ    0x03   // Загрузка файла
  #define TYPE_FILE_PROTOCOL_SEND    0x04   // Отправка файла
  #define TYPE_P2P_DOWNLOAD_PROTOCOL 0x05   // Протокол передачи файлов с помощью пиринговой сети
  #define TYPE_VOICE_PROTOCOL        0x06   // Протокол передачи голосовых сообщений
  #define TYPE_POINT_PROTOCOL        0x07   // Просто для передачи SendPacket и RecvPacket
  #define TYPE_SEND_SUPER_PACKETS    0x08   // Протокол TSSP, создан как протокол, имеющий огромный буфер, разбитый на сегменты.
                                            // Дело в том, что TLP, TSP, TPP - не могут отсылать пакеты больше ~65 кб из-за ограничений в IPv4. TSSP работает на уровне ядра
                                            // Разрезая пакеты, может использовать собственный поток, что бы резать пакеты лучше.
  #define TYPE_RUN_ASYNC_SUPER_PACKET 0x09

#endif