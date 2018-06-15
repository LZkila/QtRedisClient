#include <QtCore/QCoreApplication>
#include <QTcpSocket>
#include <QHostAddress>
#include <QDebug>
#include "RedisClient.h"
#include <QThread>
#include <QTimer>
#include <QEventLoop>
#include <QTextStream>

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	QTimer timer;
	timer.setInterval(1000);
	timer.setSingleShot(true);
	QObject::connect(&timer, &QTimer::timeout, []()
	{
		QEventLoop loop;
		QTimer t;
		t.setSingleShot(true);
		QObject::connect(&t, &QTimer::timeout, [&loop]()
		{
			QTextStream in(stdin, QIODevice::ReadOnly);
			QString str;
			RedisClient client;
			client.open(QHostAddress(QHostAddress::LocalHost).toString(), 6379);
			//qDebug() << client.get("name");
			//qDebug() << client.keys("*");
			QObject::connect(&client, &RedisClient::sigMessage, [](const RedisMsg &msg)
			{
				qDebug() << msg.value;
			});
			QObject::connect(&client, &RedisClient::sigReply, [](const RedisMsg &msg)
			{
				qDebug() << msg.value;
			});
			while (in.readLineInto(&str))
			{
				client.command(str);
				loop.processEvents();
			}
			client.close();
			loop.quit();
		});
		t.start();
		
		loop.exec();
	});
	timer.start();

	//RedisClient client;
	//client.open(QHostAddress(QHostAddress::LocalHost).toString(), 6379);
	////qDebug() << client.get("name");
	//qDebug() << client.keys("*");
	//QObject::connect(&client, &RedisClient::sigMessage, [](const RedisMsg &msg)
	//{
	//	qDebug() << msg.value;
	//});

	//QTcpSocket client;
	////client.bind(QHostAddress(QHostAddress::LocalHost), 63791);
	//client.connectToHost(QHostAddress(QHostAddress::LocalHost), 6379);
	//QObject::connect(&client, &QTcpSocket::connected, []()
	//{
	//	qDebug() << "connected";
	//});

	//QObject::connect(&client, &QTcpSocket::disconnected, []()
	//{
	//	qDebug() << "disconnected";
	//});

	//QObject::connect(&client, &QTcpSocket::readyRead, [&client]()
	//{
	//	qDebug() << "readyRead:";
	//	qDebug() << QString(client.readAll());
	//});

	//QObject::connect(&client, static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError socketError)>(&QTcpSocket::error), 
	//	[](QAbstractSocket::SocketError socketError)
	//{
	//	qDebug() << QMetaType::typeName(socketError);
	//});

	//client.write("HGETALL user\r\n");
	return a.exec();
}
