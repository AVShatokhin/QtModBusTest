#include "mb_client.h"

mb_client::mb_client(QString url, int rsTime, int retries, int adr, QObject *parent)
	: QObject(parent), _modbusServerUrl(url), _numberOfRetries(retries), _responseTime(rsTime), _deviceAddress(adr)
{
	const QUrl __url = QUrl::fromUserInput(_modbusServerUrl);

	_mb = new QModbusTcpClient();

	_mb->setConnectionParameter(QModbusDevice::NetworkPortParameter, __url.port());
	_mb->setConnectionParameter(QModbusDevice::NetworkAddressParameter, __url.host());

	_mb->setTimeout(_responseTime);
	_mb->setNumberOfRetries(_numberOfRetries);

	qDebug() << "Host = " << __url.host();
	qDebug() << "Port = " << __url.port();
	qDebug() << "responseTime = " << _responseTime;
	qDebug() << "numberOfRetries = " << _numberOfRetries;
}

mb_client::~mb_client()
{
}

void mb_client::readData()
{
	int __registerAddress = 0;
	int __registerSize = 1; // количество слов в регистре (1 слово = 2 байта)
	QModbusDataUnit __unit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, __registerAddress, __registerSize);
	startMs = QTime::currentTime().msecsSinceStartOfDay();
	QModbusReply * __reply = _mb->sendReadRequest(__unit, _deviceAddress);

	connect(__reply, &QModbusReply::finished, [=] {		
		value = __reply->result().value(0);		
		stopMs = QTime::currentTime().msecsSinceStartOfDay();
		__reply->deleteLater();
		emit readDataFinished();
	});
}

void mb_client::writeData()
{
	struct words {
		quint16 low;
		quint16 high;
	};

	union convert {
		words output;
		float input;		
	};

	startMs = QTime::currentTime().msecsSinceStartOfDay();
	
	convert __c;	
	
	valueToWrite = valueToWrite + 1.2;

	__c.input = valueToWrite;

	int __registerAddress = 2;
	int __registerSize = 2; // количество слов в регистре (1 слово = 2 байта)

	QModbusDataUnit __unit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, __registerAddress, __registerSize);
	__unit.setValue(0, __c.output.low);
	__unit.setValue(1, __c.output.high);

	QModbusReply* __reply = _mb->sendWriteRequest(__unit, _deviceAddress);

	connect(__reply, &QModbusReply::finished, [=] {		
		stopMs = QTime::currentTime().msecsSinceStartOfDay();
		count++;
		int __delta = stopMs - startMs;
		if (__delta > maxDelta) maxDelta = __delta;
		if (__delta < minDelta) minDelta = __delta;
		avgDelta += __delta;
		emit writeDataFinished();
		__reply->deleteLater();
	});
}

void mb_client::flashStats()
{
	if (count > 0)
		qDebug() << "count: " << count << " pcs, min: " << minDelta << "ms; max: " << maxDelta << "ms ; average: " << avgDelta / count << "ms; time = " << QTime().currentTime().toString();

	minDelta = 1000;
	maxDelta = 0;
	avgDelta = 0;
	count = 0;
}

void mb_client::mb_connect() {
	
	QObject::connect(_mb, &QModbusDevice::stateChanged, [=] (QModbusDevice::State state) {
		switch (_mb->state())
		{
		case QModbusDevice::UnconnectedState:
			qDebug() << "unconnected";			
			break;

		case QModbusDevice::ConnectedState:
			qDebug() << "connected";
			emit connected();
			break;

		case QModbusDevice::ConnectingState:
			qDebug() << "connecting...";
			break;

		case QModbusDevice::ClosingState:
			qDebug() << "closing...";
			break;
		}		
	});

	_mb->connectDevice();
}

