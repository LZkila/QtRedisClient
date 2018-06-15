#pragma once
#include <QObject>
#include <QVariant>

class RedisMsg
{
public:
	QString type;
	QString message;
	QString pattern;
	QString channel;
	QVariant value;
};

Q_DECLARE_METATYPE(RedisMsg)
