#pragma once
#include <QObject>
#include "RedisMsg.h"

class RedisExpress;
class RedisClient : public QObject
{
	Q_OBJECT

public:
	RedisClient(const QString &hostAddress, int port, QObject *parent = NULL);
	RedisClient(QObject *parent = NULL);

	QString getHostAddress();
	void setHostAddress(const QString &hostAddress);
	void setPort(int port);
	int getPort();
	bool isOpen();
	void open();
	void open(const QString &hostAddress, int port);
	void close();

	RedisMsg command(const QString &str);

	void publish(const QString &channel, const QString &message);
	void subscribe(const QString &channel);
	void unsubscribe(const QString &channel);
	void psubscribe(const QString &pattern);
	void punsubscribe(const QString &pattern);

	int append(const QString &key, const QString &value);
	bool auth(const QString &password);
	QStringList blpop(const QString &key, int timeout);
	QStringList brpop(const QString &key, int timeout);
	QStringList brpoplpush(const QString &source, const QString &destination, int timeout);
	int decr(const QString &key);
	int decrby(const QString &key, int interval);
	int del(const QString &key);
	QString dump(const QString &key);
	bool exists(const QString &key);
	bool expire(const QString &key, int seconds);
	bool expireat(const QString &key, int timestamp);
	QVariant eval(const QString &script, int numkeys, const QString &keys, const QString &arguments);
	QVariant evalsha(const QString &sha1, int numkeys, const QString &keys, const QString &arguments);
	QString get(const QString &key);
	QString getrange(const QString &key, int start, int stop);

	int hdel(const QString &key, const QString &field);
	bool hexists(const QString &key, const QString &field);
	QString hget(const QString &key, const QString &field);
	QMap<QString, QVariant> hgetall(const QString &key);

	int hincrby(const QString &key, const QString &field, int increment);
	float hincrbyfloat(const QString &key, const QString &field, float increment);
	QStringList hkeys(const QString &key);
	int hlen(const QString &key);
	QStringList hmget(const QString &key, const QString &field);
	bool hmset(const QString &key, QMap<QString, QVariant> map);
	bool hset(const QString &key, const QString &field, const QString &value);
	bool hsetnx(const QString &key, const QString &field, const QString &value);
	QStringList hvals(const QString &key);


	int incr(const QString &key);
	int incrby(const QString &key, int interval);
	QStringList keys(const QString & pattern);
	int lindex(const QString &key, int index);
	int linsert(const QString &key, const QString &position, const QString &pivot, const QString &value);
	int llen(const QString &key);
	QString lpop(const QString &key);
	int lpush(const QString &key, const QString &value);
	int lpushx(const QString &key, const QString &value);
	QStringList lrange(const QString &key, int start, int stop);
	int lrem(const QString &key, int count, const QString &value);
	int lset(const QString &key, int index, const QString &value);
	bool ltrim(const QString &key, int start, int stop);
	QStringList mget(const QString &key);
	bool migrate(const QString & host, int port, const QString &key, int database, int timeout);
	bool move(const QString &key, int database);
	bool mset(QMap<QString, QVariant> map);
	QVariant object(const QString &subcommand, const QString &arguments);
	bool persist(const QString &key);
	bool pexpire(const QString &key, int mseconds);
	bool pexpireat(const QString &key, int mstimestamp);
	int pttl(const QString &key);
	bool rename(const QString &key, const QString &newkey);
	bool renamex(const QString &key, const QString &newkey);
	bool restore(const QString &key, int ttl, const QString &value);
	QString rpop(const QString &key);
	QString rpoplpush(const QString &source, const QString &destination);
	int rpush(const QString &key, const QString &value);
	int rpushx(const QString &key, const QString &value);
	bool sadd(const QString &key, const QString &member);
	int scard(const QString &key);
	QStringList scriptexists(const QString &script);
	QString scriptflush();
	QString scriptkill();
	QString scriptload(const QString &script);
	QStringList sdiff(const QString &keys);
	int sdiffstore(const QString &destination, const QString &keys);
	QStringList sinter(const QString &keys);
	int sinterstore(const QString &destination, const QString &keys);
	bool sismember(const QString &key, const QString &member);
	QStringList smembers(const QString &key);
	bool smove(const QString &source, const QString &destination, const QString &member);
	QString spop(const QString &key);
	QStringList srandmember(const QString &key, int count);
	int srem(const QString &key, const QString &member);
	QStringList sunion(const QString &keys);
	int sunionstore(const QString &destination, const QString &keys);
	bool set(const QString &key, const QString &value);
	int setrange(const QString &key, int offset, const QString &value);
	QStringList sort(const QString &key, const QString &conditions);
	int ttl(const QString &key);
	QString type(const QString &key);

private:
	RedisExpress *_express = NULL;
	RedisMsg _msg;
	int _nMaxTimeOfCmd = 3000; // command最长等待时间ms 

private:
	void init();

signals:
	void sigMessage(const RedisMsg &msg);
	void sigPMessage(const RedisMsg &msg);
	void sigReply(const RedisMsg &msg);
};
