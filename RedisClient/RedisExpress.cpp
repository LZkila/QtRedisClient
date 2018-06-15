#include "RedisExpress.h"
#include <QHostAddress>
#include <QTextStream>

RedisExpress::RedisExpress(QObject *parent)
	: QObject(parent)
{
	init();
}

RedisExpress::RedisExpress(const QString &hostAddress, int port, QObject *parent)
	: QObject(parent)
{
	setHostAddress(hostAddress);
	setPort(port);
	init();
}

RedisExpress::~RedisExpress()
{

}

void RedisExpress::write(const QByteArray &data)
{
	write(QString(data));
}

void RedisExpress::write(const QString &str)
{
	if (!_socket->isValid())
		return;

	// 从字符串中分析并提取
	QChar c, nextC;
	QStringList parts;
	QString buffer;
	bool bCheck = false;
	for (int k = 0; k < str.length(); k++)
	{
		c = str.at(k);
		if (bCheck)
		{
			nextC = k < str.length() - 1 ? str
				.at(k + 1) : ' ';

			if (c == '\\' && nextC == '"') // 略过转义\"中的'\'
			{
				k++;
				continue;
			}
			else if (c == '"') // 遇到了第二个"，
			{
				bCheck = false;
			}

			buffer += c;
		}
		else
		{
			if (!c.isSpace())
			{
				if (c == '\\' && nextC == '"') // 略过转义\"中的'\'
				{
					k++;
					continue;
				}
				else if (c == '"') // 遇到第一个"
				{
					bCheck = true;
				}

				buffer += c;
			}
			else if (!buffer.isEmpty())
			{
				parts << buffer; // 追加一个单词
				buffer.clear();
			}
		}
	}

	if (!buffer.isEmpty()) // 当最后一个字母不是' '也不是'"'时
	{
		parts << buffer;
	}

	// 准备符合redis服务器接收的格式的数据
	QString sendData;
	sendData.append(QString("*%1\r\n").arg(parts.length())); // 总长度
	for (int i = 0; i < parts.length(); i++)
	{
		sendData.append(QString("$%1\r\n").arg(parts.at(i).length())); // 单词长度
		sendData.append(QString("%1\r\n").arg(parts.at(i))); // 单词
	}

	// 发送
	QTextStream stream(_socket);
	stream << sendData;
	stream.flush();
}

QStringList RedisExpress::parseLine(const QByteArray &data)
{
	if (data.isEmpty())
	{
		return QStringList();
	}

	QChar first = data.at(0);
	QString value;
	QStringList result;

	if (first == '+') // 字符串
	{
		value = data.mid(1);
		value.chop(2); // 删除\r\n

		result << "string" << value;
	}
	else if (first == '-') // 错误
	{
		value = data.mid(1);
		value.chop(2);

		result << "error" << value;
	}
	else if (first == ':') // 整型
	{
		value = data.mid(1);
		value.chop(2);

		result << "integer" << value;

	}
	else if (first == '$') // 字符串
	{
		auto index = data.indexOf("\r\n");
		auto len = data.mid(1, index - 1).toInt();
		if (len == -1)
			value = "NULL";
		else
			value = data.mid(index + 2, len);

		result << "bulk" << value;
	}
	else if (first == '*') // 列表
	{
		auto index = data.indexOf("\r\n");
		auto count = data.mid(1, index - 1).toInt(); // 列表中元素个数

		int len = -1;
		auto pos = index + 2; // 第一个元素索引
		result << "list";

		for (int i = 0; i < count; i++)
		{
			index = data.indexOf("\r\n", pos);
			len = data.mid(pos + 1, index - pos - 1).toInt();
			if (len == -1)
				result << "NULL";
			else
				result << data.mid(index + 2, len); // 提取并追加元素

			pos = index + 2 + len + 2; // 下一个元素索引
		}
	}

	return result;
}

void RedisExpress::bind(const QString &address, int port)
{
	_socket->bind(QHostAddress(address), port);
}

bool RedisExpress::isOpen()
{
	return _socket->isValid();
}

void RedisExpress::setHostAddress(const QString &hostAddress)
{
	_hostAddress.setAddress(hostAddress);
	if (_socket->isValid()) // 如果已连接，重连新的地址
	{
		open(_hostAddress, _nPort);
	}
}

void RedisExpress::setPort(int port)
{
	_nPort = port;
	if (_socket->isValid())
	{
		open(_hostAddress, _nPort);
	}
}

void RedisExpress::open()
{
	open(_hostAddress, _nPort);
}

void RedisExpress::open(const QString &hostAddress, int port)
{
	open(QHostAddress(hostAddress), port);
}

void RedisExpress::open(const QHostAddress & hostAddress, int port)
{
	_hostAddress = hostAddress;
	_nPort = port;
	_socket->connectToHost(hostAddress, port);
}

void RedisExpress::close()
{
	_socket->disconnectFromHost();
}

void RedisExpress::init()
{
	_socket = new QTcpSocket(this);
	connect(_socket, &QTcpSocket::connected, this, &RedisExpress::sigConnected);
	connect(_socket, &QTcpSocket::disconnected, this, &RedisExpress::sigDisconnected);
	connect(_socket, &QTcpSocket::readyRead, [this]()
	{
		auto sl = parseLine(_socket->readAll()); // 解析
		emit sigReply(sl);
	});
	connect(_socket, static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError socketError)>(&QTcpSocket::error),
		[this](QAbstractSocket::SocketError socketError)
	{
		QString error(QMetaType::typeName(socketError));
		qDebug() << error; // 错误
		emit sigError(error);
	});

	//connect(_socket, &QTcpSocket::connected, []()
	//{
	//	qDebug() << "connected";
	//});

	//connect(_socket, &QTcpSocket::disconnected, []()
	//{
	//	qDebug() << "disconnected";
	//});
}
