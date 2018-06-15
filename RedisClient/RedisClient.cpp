#include "RedisClient.h"
#include <QEventLoop>
#include <QTimer>

#include "RedisExpress.h"

RedisClient::RedisClient(QObject *parent)
	: QObject(parent)
{
	init();
}

RedisClient::RedisClient(const QString &hostAddress, int port, QObject *parent)
	: QObject(parent)
{
	init();
	setHostAddress(hostAddress);
	setPort(port);
}

QString RedisClient::getHostAddress()
{
	return _express->getHostAddress();
}

void RedisClient::setHostAddress(const QString &hostAddress)
{
	Q_ASSERT(_express != NULL);
	_express->setHostAddress(hostAddress);
}

void RedisClient::setPort(int port)
{
	Q_ASSERT(_express != NULL);
	_express->setPort(port);
}

int RedisClient::getPort()
{
	return _express->getPort();
}

bool RedisClient::isOpen()
{
	return _express->isOpen();
}

void RedisClient::open()
{
	_express->open();
}

void RedisClient::open(const QString &hostAddress, int port)
{
	_express->open(hostAddress, port);
}

void RedisClient::close()
{
	_express->close();
}

RedisMsg RedisClient::command(const QString & str)
{
	_express->write(str);

	QEventLoop loop;
	connect(_express, &RedisExpress::sigReply, [&loop](const QStringList &res)
	{
		loop.quit();
	});
	
	QTimer t;
	t.setInterval(_nMaxTimeOfCmd);
	t.setSingleShot(true);
	bool bTimeout = false;
	connect(&t, &QTimer::timeout, [&loop, &bTimeout]() // 超时处理
	{
		loop.quit();
		bTimeout = true;
	});
	t.start();

	loop.exec();

	if (bTimeout)
		return RedisMsg();

	return _msg;
}

void RedisClient::publish(const QString & channel, const QString & message)
{
	QString cmd;
	cmd.append("PUBLISH ");
	cmd.append(channel);
	cmd.append(" ");
	cmd.append(message);

	_express->write(cmd);
}

void RedisClient::subscribe(const QString & channel)
{
	QString cmd;
	cmd.append("SUBSCRIBE ");
	cmd.append(channel);

	_express->write(cmd);
}

void RedisClient::unsubscribe(const QString & channel)
{
	QString cmd;
	cmd.append("UNSUBSCRIBE ");
	cmd.append(channel);

	_express->write(cmd);
}

void RedisClient::psubscribe(const QString & pattern)
{
	QString cmd;
	cmd.append("PSUBSCRIBE ");
	cmd.append(pattern);

	_express->write(cmd);
}

void RedisClient::punsubscribe(const QString & pattern)
{
	QString cmd;
	cmd.append("PUNSUBSCRIBE ");
	cmd.append(pattern);

	_express->write(cmd);
}

int RedisClient::append(const QString & key, const QString & value)
{
	QString cmd("APPEND ");
	cmd.append(key);
	cmd.append(" ");
	cmd.append(value);

	auto reply = command(cmd);

	return reply.value.toInt();
}

bool RedisClient::auth(const QString & password)
{
	QString cmd("AUTH ");
	cmd.append(password);

	auto reply = command(cmd);
	if (reply.value.toString() == "OK")
		return true;
	return false;
}

QStringList RedisClient::blpop(const QString & key, int timeout)
{
	QString cmd("BLPOP ");
	cmd.append(key);
	cmd.append(" ");
	cmd.append(QString::number(timeout));

	auto reply = command(cmd);
	return reply.value.toStringList();
}

QStringList RedisClient::brpop(const QString & key, int timeout)
{
	QString cmd("BRPOP ");
	cmd.append(key);
	cmd.append(" ");
	cmd.append(QString::number(timeout));

	auto reply = command(cmd);
	return reply.value.toStringList();
}

QStringList RedisClient::brpoplpush(const QString & source, const QString & destination, int timeout)
{
	QString cmd("BRPOPLPUSH ");
	cmd.append(source);
	cmd.append(" ");
	cmd.append(destination);
	cmd.append(" ");
	cmd.append(QString::number(timeout));

	auto reply = command(cmd);
	return reply.value.toStringList();
}

int RedisClient::decr(const QString & key)
{
	QString cmd("DECR ");
	cmd.append(key);

	auto reply = command(cmd);
	return reply.value.toInt();
}

int RedisClient::decrby(const QString & key, int interval)
{
	QString cmd("DECRBY ");
	cmd.append(key);
	cmd.append(" ");
	cmd.append(QString::number(interval));

	auto reply = command(cmd);
	return reply.value.toInt();
}

int RedisClient::del(const QString & key)
{
	QString cmd("DEL ");
	cmd.append(key);

	auto reply = command(cmd);
	return reply.value.toInt();
}

QString RedisClient::dump(const QString & key)
{
	QString cmd("DUMP ");
	cmd.append(key);

	auto reply = command(cmd);
	return reply.value.toString();
}

bool RedisClient::exists(const QString & key)
{
	QString cmd("EXISTS ");
	cmd.append(key);

	auto reply = command(cmd);
	return reply.value.toBool();
}

bool RedisClient::expire(const QString & key, int seconds)
{
	QString cmd("EXPIRE ");
	cmd.append(key);
	cmd.append(" ");
	cmd.append(QString::number(seconds));

	auto reply = command(cmd);
	return reply.value.toBool();
}

bool RedisClient::expireat(const QString & key, int timestamp)
{
	QString cmd("EXPIREAT ");
	cmd.append(key);
	cmd.append(" ");
	cmd.append(QString::number(timestamp));

	auto reply = command(cmd);
	return reply.value.toBool();
}

QVariant RedisClient::eval(const QString & script, int numkeys, const QString & keys, const QString & arguments)
{
	QString cmd("EVAL ");
	cmd.append(script);
	cmd.append(" ");
	cmd.append(QString::number(numkeys));
	cmd.append(" ");
	cmd.append(keys);
	cmd.append(" ");
	cmd.append(arguments);

	auto reply = command(cmd);
	return reply.value;
}

QVariant RedisClient::evalsha(const QString & sha1, int numkeys, const QString & keys, const QString & arguments)
{
	QString cmd("EVALSHA ");
	cmd.append(sha1);
	cmd.append(" ");
	cmd.append(QString::number(numkeys));
	cmd.append(" ");
	cmd.append(keys);
	cmd.append(" ");
	cmd.append(arguments);

	auto reply = command(cmd);
	return reply.value;
}

QString RedisClient::get(const QString & key)
{
	QString cmd("GET ");
	cmd.append(key);

	auto reply = command(cmd);
	return reply.value.toString();
}

QString RedisClient::getrange(const QString & key, int start, int stop)
{
	QString cmd("GETRANGE ");
	cmd.append(key);
	cmd.append(" ");
	cmd.append(QString::number(start));
	cmd.append(" ");
	cmd.append(QString::number(stop));

	auto reply = command(cmd);
	return reply.value.toString();
}

int RedisClient::hdel(const QString & key, const QString & field)
{
	QString cmd("HDEL ");
	cmd.append(key);
	cmd.append(" ");
	cmd.append(field);

	auto reply = command(cmd);
	return reply.value.toInt();
}

bool RedisClient::hexists(const QString & key, const QString & field)
{
	QString cmd("HEXISTS ");
	cmd.append(key);
	cmd.append(" ");
	cmd.append(field);

	auto reply = command(cmd);
	return reply.value.toBool();
}

QString RedisClient::hget(const QString & key, const QString & field)
{
	QString cmd("HGET ");
	cmd.append(key);
	cmd.append(" ");
	cmd.append(field);

	auto reply = command(cmd);
	return reply.value.toString();
}

QMap<QString, QVariant> RedisClient::hgetall(const QString & key)
{
	QString cmd("HGETALL ");
	cmd.append(key);

	auto reply = command(cmd);

	QMap<QString, QVariant> keypairs;
	QStringList list = reply.value.toStringList();

	for (int i = 0; i<list.length(); i++)
	{
		QString k = list[i];
		i++;
		QString v = list[i];

		keypairs[k] = QString(v);
	}

	return keypairs;
}

int RedisClient::hincrby(const QString & key, const QString & field, int increment)
{
	QString cmd("HINCRBY ");
	cmd.append(key);
	cmd.append(" ");
	cmd.append(field);
	cmd.append(" ");
	cmd.append(QString::number(increment));

	auto reply = command(cmd);
	return reply.value.toInt();
}

float RedisClient::hincrbyfloat(const QString & key, const QString & field, float increment)
{
	QString cmd("HINCRBYFLOAT ");
	cmd.append(key);
	cmd.append(" ");
	cmd.append(field);
	cmd.append(" ");
	cmd.append(QString::number(increment));

	auto reply = command(cmd);
	return reply.value.toFloat();
}

QStringList RedisClient::hkeys(const QString & key)
{
	QString cmd("HKEYS ");
	cmd.append(key);

	auto reply = command(cmd);
	return reply.value.toStringList();
}

int RedisClient::hlen(const QString & key)
{
	QString cmd("HLEN ");
	cmd.append(key);

	auto reply = command(cmd);
	return reply.value.toInt();
}

QStringList RedisClient::hmget(const QString & key, const QString & field)
{
	QString cmd("HMGET ");
	cmd.append(key);
	cmd.append(" ");
	cmd.append(field);

	auto reply = command(cmd);
	return reply.value.toStringList();
}

bool RedisClient::hmset(const QString & key, QMap<QString, QVariant> map)
{
	QString cmd("HMSET ");
	cmd.append(key);

	QMapIterator<QString, QVariant> i(map);
	while (i.hasNext())
	{
		i.next();
		cmd.append(" ");
		cmd.append(i.key());
		cmd.append(" ");
		cmd.append(i.value().toString());

	}

	auto reply = command(cmd);
	if (reply.value.toString() == "OK")
		return true;

	return false;
}

bool RedisClient::hset(const QString & key, const QString & field, const QString & value)
{
	QString cmd("HSET ");
	cmd.append(key);
	cmd.append(" ");
	cmd.append(field);
	cmd.append(" ");
	cmd.append(value);

	auto reply = command(cmd);
	return reply.value.toBool();
}

bool RedisClient::hsetnx(const QString & key, const QString & field, const QString & value)
{
	QString cmd("HSETNX ");
	cmd.append(key);
	cmd.append(" ");
	cmd.append(field);
	cmd.append(" ");
	cmd.append(value);

	auto reply = command(cmd);
	return reply.value.toBool();
}

QStringList RedisClient::hvals(const QString & key)
{
	QString cmd("HVALS ");
	cmd.append(key);

	auto reply = command(cmd);
	return reply.value.toStringList();
}

int RedisClient::incr(const QString & key)
{
	QString cmd("INCR ");
	cmd.append(key);

	auto reply = command(cmd);
	return reply.value.toInt();
}

int RedisClient::incrby(const QString & key, int interval)
{
	QString cmd("INCRBY ");
	cmd.append(key);
	cmd.append(" ");
	cmd.append(QString::number(interval));

	auto reply = command(cmd);
	return reply.value.toInt();
}

QStringList RedisClient::keys(const QString & pattern)
{
	QString cmd("KEYS ");
	cmd.append(pattern);

	auto reply = command(cmd);
	return reply.value.toStringList();
}

int RedisClient::lindex(const QString & key, int index)
{
	QString cmd("LINDEX ");
	cmd.append(key);
	cmd.append(" ");
	cmd.append(QString::number(index));

	auto reply = command(cmd);
	return reply.value.toInt();
}

int RedisClient::linsert(const QString & key, const QString & position, const QString & pivot, const QString & value)
{
	QString cmd("LINSERT ");
	cmd.append(key);
	cmd.append(" ");
	cmd.append(position);
	cmd.append(" ");
	cmd.append(pivot);
	cmd.append(" ");
	cmd.append(value);

	auto reply = command(cmd);
	return reply.value.toInt();
}

int RedisClient::llen(const QString & key)
{
	QString cmd("LLEN ");
	cmd.append(key);

	auto reply = command(cmd);
	return reply.value.toInt();
}

QString RedisClient::lpop(const QString & key)
{
	QString cmd("LPOP ");
	cmd.append(key);

	auto reply = command(cmd);
	return reply.value.toString();
}

int RedisClient::lpush(const QString & key, const QString & value)
{
	QString cmd("LPUSH ");
	cmd.append(key);
	cmd.append(" ");
	cmd.append(value);

	auto reply = command(cmd);
	return reply.value.toInt();
}

int RedisClient::lpushx(const QString & key, const QString & value)
{
	QString cmd("LPUSHX ");
	cmd.append(key);
	cmd.append(" ");
	cmd.append(value);

	auto reply = command(cmd);
	return reply.value.toInt();
}

QStringList RedisClient::lrange(const QString & key, int start, int stop)
{
	QString cmd("LRANGE ");
	cmd.append(key);
	cmd.append(" ");
	cmd.append(QString::number(start));
	cmd.append(" ");
	cmd.append(QString::number(stop));

	auto reply = command(cmd);
	return reply.value.toStringList();
}

int RedisClient::lrem(const QString & key, int count, const QString & value)
{
	QString cmd("LREM ");
	cmd.append(key);
	cmd.append(" ");
	cmd.append(QString::number(count));
	cmd.append(" ");
	cmd.append(value);

	auto reply = command(cmd);
	return reply.value.toInt();
}

int RedisClient::lset(const QString & key, int index, const QString & value)
{
	QString cmd("LSET ");
	cmd.append(key);
	cmd.append(" ");
	cmd.append(QString::number(index));
	cmd.append(" ");
	cmd.append(value);

	auto reply = command(cmd);
	return reply.value.toInt();
}

bool RedisClient::ltrim(const QString & key, int start, int stop)
{
	QString cmd("LTRIM ");
	cmd.append(key);
	cmd.append(" ");
	cmd.append(QString::number(start));
	cmd.append(" ");
	cmd.append(QString::number(stop));

	auto reply = command(cmd);

	if (reply.value.toString() == "OK")
		return true;
	return false;
}

QStringList RedisClient::mget(const QString & key)
{
	QString cmd("MGET ");
	cmd.append(key);

	auto reply = command(cmd);
	return reply.value.toStringList();
}

bool RedisClient::migrate(const QString & host, int port, const QString & key, int database, int timeout)
{
	QString cmd("MIGRATE ");
	cmd.append(host);
	cmd.append(" ");
	cmd.append(QString::number(port));
	cmd.append(" ");
	cmd.append(key);
	cmd.append(" ");
	cmd.append(QString::number(database));
	cmd.append(" ");
	cmd.append(QString::number(timeout));

	auto reply = command(cmd);

	if (reply.value.toString() == "OK")
		return true;
	return false;
}

bool RedisClient::move(const QString & key, int database)
{
	QString cmd("MOVE ");
	cmd.append(key);
	cmd.append(" ");
	cmd.append(QString::number(database));

	auto reply = command(cmd);
	return reply.value.toBool();
}

bool RedisClient::mset(QMap<QString, QVariant> map)
{
	QString cmd("MSET");

	QMapIterator<QString, QVariant> i(map);
	while (i.hasNext())
	{
		i.next();
		cmd.append(" ");
		cmd.append(i.key());
		cmd.append(" ");
		cmd.append(i.value().toString());

	}

	auto reply = command(cmd);

	if (reply.value.toString() == "OK")
		return true;
	return false;
}

QVariant RedisClient::object(const QString & subcommand, const QString & arguments)
{
	QString cmd("OBJECT ");
	cmd.append(subcommand);
	cmd.append(" ");
	cmd.append(arguments);

	auto reply = command(cmd);
	return reply.value;
}

bool RedisClient::persist(const QString & key)
{
	QString cmd("PERSIST ");
	cmd.append(key);

	auto reply = command(cmd);
	return reply.value.toBool();
}

bool RedisClient::pexpire(const QString & key, int mseconds)
{
	QString cmd("PEXPIRE ");
	cmd.append(key);
	cmd.append(" ");
	cmd.append(QString::number(mseconds));

	auto reply = command(cmd);
	return reply.value.toBool();
}

bool RedisClient::pexpireat(const QString & key, int mstimestamp)
{
	QString cmd("PEXPIREAT ");
	cmd.append(key);
	cmd.append(" ");
	cmd.append(QString::number(mstimestamp));

	auto reply = command(cmd);
	return reply.value.toBool();
}

int RedisClient::pttl(const QString & key)
{
	QString cmd("PTTL ");
	cmd.append(key);

	auto reply = command(cmd);
	return reply.value.toInt();
}

bool RedisClient::rename(const QString & key, const QString & newkey)
{
	QString cmd("RENAME ");
	cmd.append(key);
	cmd.append(" ");
	cmd.append(newkey);

	auto reply = command(cmd);

	if (reply.value.toString() == "OK")
		return true;
	return false;
}

bool RedisClient::renamex(const QString & key, const QString & newkey)
{
	QString cmd("RENAMEX ");
	cmd.append(key);
	cmd.append(" ");
	cmd.append(newkey);

	auto reply = command(cmd);

	if (reply.value.toString() == "OK")
		return true;
	return false;
}

bool RedisClient::restore(const QString & key, int ttl, const QString & value)
{
	QString cmd("RESTORE ");
	cmd.append(key);
	cmd.append(" ");
	cmd.append(QString::number(ttl));
	cmd.append(" ");
	cmd.append(value);

	auto reply = command(cmd);

	if (reply.value.toString() == "OK")
		return true;
	return false;
}

QString RedisClient::rpop(const QString & key)
{
	QString cmd("RPOP ");
	cmd.append(key);

	auto reply = command(cmd);
	return reply.value.toString();
}

QString RedisClient::rpoplpush(const QString & source, const QString & destination)
{
	QString cmd("RPOPLPUSH ");
	cmd.append(source);
	cmd.append(" ");
	cmd.append(destination);

	auto reply = command(cmd);
	return reply.value.toString();
}

int RedisClient::rpush(const QString & key, const QString & value)
{
	QString cmd("RPUSH ");
	cmd.append(key);
	cmd.append(" ");
	cmd.append(value);

	auto reply = command(cmd);
	return reply.value.toInt();
}

int RedisClient::rpushx(const QString & key, const QString & value)
{
	QString cmd("RPUSHX ");
	cmd.append(key);
	cmd.append(" ");
	cmd.append(value);

	auto reply = command(cmd);
	return reply.value.toInt();
}

bool RedisClient::sadd(const QString & key, const QString & member)
{
	QString cmd("SADD ");
	cmd.append(key);
	cmd.append(" ");
	cmd.append(member);

	auto reply = command(cmd);
	return reply.value.toBool();
}

int RedisClient::scard(const QString & key)
{
	QString cmd("SCARD ");
	cmd.append(key);

	auto reply = command(cmd);
	return reply.value.toInt();
}

QStringList RedisClient::scriptexists(const QString & script)
{
	QString cmd("SCRIPT EXISTS ");
	cmd.append(script);

	auto reply = command(cmd);
	return reply.value.toStringList();
}

QString RedisClient::scriptflush()
{
	QString cmd("SCRIPT FLUSH");

	auto reply = command(cmd);
	return reply.value.toString();
}

QString RedisClient::scriptkill()
{
	QString cmd("SCRIPT KILL");

	auto reply = command(cmd);
	return reply.value.toString();
}

QString RedisClient::scriptload(const QString & script)
{
	QString cmd("SCRIPT LOAD ");
	cmd.append(script);

	auto reply = command(cmd);
	return reply.value.toString();
}

QStringList RedisClient::sdiff(const QString & keys)
{
	QString cmd("SDIFF ");
	cmd.append(keys);

	auto reply = command(cmd);
	return reply.value.toStringList();
}

int RedisClient::sdiffstore(const QString & destination, const QString & keys)
{
	QString cmd("SDIFFSTORE ");
	cmd.append(destination);
	cmd.append(" ");
	cmd.append(keys);

	auto reply = command(cmd);
	return reply.value.toInt();
}

QStringList RedisClient::sinter(const QString & keys)
{
	QString cmd("SINTER ");
	cmd.append(keys);

	auto reply = command(cmd);
	return reply.value.toStringList();
}

int RedisClient::sinterstore(const QString & destination, const QString & keys)
{
	QString cmd("SINTERSTORE ");
	cmd.append(destination);
	cmd.append(" ");
	cmd.append(keys);

	auto reply = command(cmd);
	return reply.value.toInt();
}

bool RedisClient::sismember(const QString & key, const QString & member)
{
	QString cmd("SISMEMBER ");
	cmd.append(key);
	cmd.append(" ");
	cmd.append(member);

	auto reply = command(cmd);
	return reply.value.toBool();
}

QStringList RedisClient::smembers(const QString & key)
{
	QString cmd("SMEMBERS ");
	cmd.append(key);

	auto reply = command(cmd);
	return reply.value.toStringList();
}

bool RedisClient::smove(const QString & source, const QString & destination, const QString & member)
{
	QString cmd("SMOVE ");
	cmd.append(source);
	cmd.append(" ");
	cmd.append(destination);
	cmd.append(" ");
	cmd.append(member);

	auto reply = command(cmd);
	return reply.value.toBool();
}

QString RedisClient::spop(const QString & key)
{
	QString cmd("SPOP ");
	cmd.append(key);

	auto reply = command(cmd);
	return reply.value.toString();
}

QStringList RedisClient::srandmember(const QString & key, int count)
{
	QString cmd("SRANDMEMBER ");
	cmd.append(key);
	cmd.append(" ");
	cmd.append(QString::number(count));

	auto reply = command(cmd);
	return reply.value.toStringList();
}

int RedisClient::srem(const QString & key, const QString & member)
{
	QString cmd("SREM ");
	cmd.append(key);
	cmd.append(" ");
	cmd.append(member);

	auto reply = command(cmd);
	return reply.value.toInt();
}

QStringList RedisClient::sunion(const QString & keys)
{
	QString cmd("SUNION ");
	cmd.append(keys);

	auto reply = command(cmd);
	return reply.value.toStringList();
}

int RedisClient::sunionstore(const QString & destination, const QString & keys)
{
	QString cmd("SUNIONSTORE ");
	cmd.append(destination);
	cmd.append(" ");
	cmd.append(keys);

	auto reply = command(cmd);
	return reply.value.toInt();
}

bool RedisClient::set(const QString & key, const QString & value)
{
	QString cmd("SET ");
	cmd.append(key);
	cmd.append(" ");
	cmd.append(value);

	auto reply = command(cmd);

	if (reply.value.toString() == "OK")
		return true;
	return false;
}

int RedisClient::setrange(const QString & key, int offset, const QString & value)
{
	QString cmd("SETRANGE ");
	cmd.append(key);
	cmd.append(" ");
	cmd.append(QString::number(offset));
	cmd.append(" ");
	cmd.append(value);

	auto reply = command(cmd);
	return reply.value.toInt();
}

QStringList RedisClient::sort(const QString & key, const QString & conditions)
{
	QString cmd("SORT ");
	cmd.append(key);
	cmd.append(" ");
	cmd.append(conditions);

	auto reply = command(cmd);
	return reply.value.toStringList();
}

int RedisClient::ttl(const QString & key)
{
	QString cmd("TTL ");
	cmd.append(key);

	auto reply = command(cmd);
	return reply.value.toInt();
}

QString RedisClient::type(const QString & key)
{
	QString cmd("TYPE ");
	cmd.append(key);

	auto reply = command(cmd);
	return reply.value.toString();
}

void RedisClient::init()
{
	_express = new RedisExpress(this);
	connect(_express, &RedisExpress::sigReply, [this](const QStringList &res)
	{
		if (res[1] == "message")
		{
			_msg.type = res[0];
			_msg.message = res[1];
			_msg.channel = res[2];
			_msg.value = res[3];

			emit sigMessage(_msg);
		}
		else if (res[1] == "pmessage")
		{
			_msg.type = res[0];
			_msg.message = res[1];
			_msg.pattern = res[2];
			_msg.channel = res[3];
			_msg.value = res[4];

			emit sigPMessage(_msg);
		}
		else
		{
			if (res[0] == "integer")
			{
				_msg.type = res[0];
				_msg.value = res[1].toInt();
			}
			else if (res[0] == "list")
			{
				_msg.type = res[0];

				QStringList list;
				for (int i = 1; i < res.length(); i++)
				{
					list << res[i];
				}

				_msg.value = list;
			}
			else
			{
				_msg.type = res[0];
				_msg.value = res[1];
			}

			emit sigReply(_msg);
		}
	});
}
