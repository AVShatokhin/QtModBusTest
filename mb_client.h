#pragma once

#include <QObject>

#include <QModbusTcpClient>
#include <QModbusDataUnit>

#include <QVariant>
#include <QUrl>
#include <QTime>

class mb_client : public QObject
{
	Q_OBJECT

public:
	mb_client(QString url, int rsTime, int retries, int adr, QObject* parent = nullptr);
	~mb_client();
	int startMs = 0;
	int stopMs = 0;
	quint16 value = 0;
	float valueToWrite = 0.12;
	int minDelta = 1000;
	int maxDelta = 0;
	int avgDelta = 0;
	int count = 0;

public slots:
	void mb_connect();
	void readData();
	void writeData();
	void flashStats();

private:	
	QString _modbusServerUrl;
	int _responseTime;
	int _numberOfRetries;
	int _deviceAddress;
	QModbusTcpClient * _mb;

signals:
	void connected();
	void readDataFinished();
	void writeDataFinished();
};
