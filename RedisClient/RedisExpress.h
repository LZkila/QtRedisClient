#pragma once
#include <QObject>
#include <QByteArray>
#include <QTcpSocket>
#include <QHostAddress>

class RedisExpress : public QObject
{
	Q_OBJECT

public:
	RedisExpress(const QString &hostAddress, int port, QObject *parent = NULL);
	RedisExpress(QObject *parent = NULL);
	~RedisExpress();

	void write(const QByteArray &data); // 发送
	void write(const QString &str);
	void bind(const QString &address, int port);
	bool isOpen();
	void open(); // 连接
	void open(const QString &hostAddress, int port);
	void open(const QHostAddress &hostAddress, int port);
	void close(); // 关闭

	QString getHostAddress() { return _hostAddress.toString(); }
	void setHostAddress(const QString &hostAddress);

	int getPort() { return _nPort; }
	void setPort(int port);

private:
	QHostAddress _hostAddress;
	int _nPort = 0;
	QTcpSocket *_socket = NULL;

private:
	QStringList parseLine(const QByteArray &data); // 解析
	void init();

signals:
	void sigConnected(); // 已连上
	void sigDisconnected(); // 已断开
	void sigError(const QString &sError); // 错误
	void sigReply(const QStringList &sl); // 回复
};