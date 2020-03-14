#include "remoteclient.h"




RemoteClient::RemoteClient(QObject *parent)
    : QObject(parent)
{
   m_Timer = new QTimer(this);
   connect( m_Timer , SIGNAL(timeout()), this , SLOT(Update()));

   m_Socket = new QUdpSocket(this);
   m_Port = 8888;
   m_CoffTurn = 1;
}

RemoteClient::~RemoteClient()
{
   if(m_Timer)  delete m_Timer;
   if(m_Socket) delete  m_Socket;
}

QString RemoteClient::textInput()
{
    return m_textInput;
}

void RemoteClient::setTextInput(const QString &textInput)
{
    if(m_textInput == textInput ) return;

    m_textInput = textInput;
    emit textInputChanged();
}

void RemoteClient::speed_PWM(int speed)
{
    m_Speed = speed;
}

void RemoteClient::speed_Turn(int speed_turn)
{
    m_Turn = speed_turn;
}

void RemoteClient::enabled_LED(bool enabled)
{
    packet_remote.enLED = enabled;
}

void RemoteClient::coffTurn(int _coff_trun)
{
    _coff_trun = (_coff_trun == 0) ? 1 : _coff_trun;
    m_CoffTurn = (_coff_trun >= 0.) ? std::abs(_coff_trun) : 1.0 / std::abs(_coff_trun);

    qDebug() << m_CoffTurn;
}

bool RemoteClient::onConnect(QString _ip_address )
{
    if(!m_Timer->isActive())
    {
       // включение таймера
       m_IPAddress = QHostAddress(_ip_address);
       m_Timer->start(10);
       return true;
    }
    else
    {
       // выключение таймера
       m_Speed=0;
       m_Turn =0;
       packet_remote=DataPacketRemote(0,0,0);
       m_Timer->stop();
       Update();
       return false;
    }

}

void RemoteClient::Update()
{
    // Формула контроля движения робокара
    packet_remote.speed_PWM_X = int(m_Speed - m_Turn * m_CoffTurn);
    packet_remote.speed_PWM_Y = int(m_Speed + m_Turn * m_CoffTurn);

    // отсылаем пакеты данных на робокар
    m_Socket->writeDatagram( (char*)&packet_remote , sizeof(DataPacketRemote) , m_IPAddress , m_Port);

    // принимаем пакеты данные от робокара
    const qint64 MAX_SIZE_BUFFER = 255;//sizeof(DataPacketRemote)+1;
    char buff_read[MAX_SIZE_BUFFER] = "no packet";
    qint64 nsize = m_Socket->readDatagram( buff_read , MAX_SIZE_BUFFER );
    buff_read[nsize] = 0;

    // Вывводим только 10 строк последних сообщений от робокара
    if( m_NSizeLines < 10 )
    {
       setTextInput(QString(textInput()) + QString("  ") + QString(buff_read));
    }
    else
    {
      // Очищаем консоль выввода данных от робокара
      setTextInput("");
      m_NSizeLines = 0;
    }

    // сщетчик строк
    m_NSizeLines++;
}


