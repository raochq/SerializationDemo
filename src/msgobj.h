#ifndef MSGOBJ_H
#define MSGOBJ_H

#include <map>
#include <list>
#include <algorithm>
#include <utility>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

#define JsonWriteBegin(writer) \
	{                          \
		writer.StartObject();
#define JsonWriteEnd()  \
	writer.EndObject(); \
	}

#define JsonWrite(XXX) \
	writer.Key(#XXX);  \
	WriteEvery(writer, XXX);

#define JsonParseBegin(val)                                           \
	for (auto itr = val.MemberBegin(); itr != val.MemberEnd(); ++itr) \
	{
#define JsonParseEnd() }

#define JsonParse(XXX)                            \
	if (strcmp(itr->name.GetString(), #XXX) == 0) \
		ParseEvery(itr->value, XXX);

#define REGISTER_CLASS(id, T) \
	const int T::ClassId = Class::Register<T>(id, #T)->GetClassId();

#define RTTIWRITE(_parent, _mems)                               \
	static const int ClassId;                                   \
	virtual int GetClassId() const override { return ClassId; } \
	void WriteMember(Writer<StringBuffer> &writer)              \
	{                                                           \
		_parent::WriteMember(writer);                           \
		_mems;                                                  \
	}

#define RTTIPARSE(_parent, _mems)                         \
	void ReadMember(const Value::ConstMemberIterator itr) \
	{                                                     \
		_parent::ReadMember(itr);                         \
		_mems;                                            \
	}

using namespace rapidjson;
class MsgObj
{
  public:
	MsgObj() {}
	~MsgObj() {}
	std::string ToString()
	{
		StringBuffer s;
		Writer<StringBuffer> writer(s);
		this->ToJson(writer);
		return s.GetString();
	}
	static MsgObj *CreateObject(const std::string &json);

	void ToJson(Writer<StringBuffer> &writer)
	{
		JsonWriteBegin(writer);
		this->WriteMember(writer);
		JsonWriteEnd();
	}

	void ParseJson(const std::string &json)
	{
		Document dom;
		dom.Parse(json.data());
		this->ParseJson(dom);
	}

	void ParseJson(const Value &val)
	{
		JsonParseBegin(val);
		ReadMember(itr);
		JsonParseEnd();
	}

	virtual int GetClassId() const { return ClassId; }
	static const int ClassId = 0;

  protected:
	virtual void WriteMember(Writer<StringBuffer> &writer)
	{
		writer.Key("ClassId");
		writer.Int(GetClassId());
	}
	virtual void ReadMember(const Value::ConstMemberIterator itr) {};
	template <typename T>
	static void WriteEvery(Writer<StringBuffer> &writer, T &val)
	{
		MsgObj *p = &val;
		p->ToJson(writer);
	}

	template <typename T>
	static void WriteEvery(Writer<StringBuffer> &writer, std::list<T> &val)
	{
		writer.StartArray();
		for (auto ent : val)
		{
			WriteEvery(writer, ent);
		}
		writer.EndArray();
	}

	static void WriteEvery(Writer<StringBuffer> &writer, int32_t &val) { writer.Int(val); }
	static void WriteEvery(Writer<StringBuffer> &writer, int64_t &val) { writer.Int64(val); }
	static void WriteEvery(Writer<StringBuffer> &writer, uint32_t &val) { writer.Uint(val); }
	static void WriteEvery(Writer<StringBuffer> &writer, uint64_t &val) { writer.Uint64(val); }
	static void WriteEvery(Writer<StringBuffer> &writer, double &val) { writer.Double(val); }
	static void WriteEvery(Writer<StringBuffer> &writer, bool &val) { writer.Bool(val); }
	static void WriteEvery(Writer<StringBuffer> &writer, std::string &val) { writer.String(val.data(), val.length()); }
	static void WriteEvery(Writer<StringBuffer> &writer, char *val) { writer.String(val, strlen(val)); }

	template <typename T>
	static void ParseEvery(const Value &val, T &t)
	{
		MsgObj *p = &t;
		p->ParseJson(val);
	}
	template <typename T>
	static void ParseEvery(const Value &val, std::list<T> &t)
	{
		SizeType len = val.Size();
		for (size_t i = 0; i < len; i++)
		{
			const Value &f = val[i];
			//decltype(t)::value_type obj;
			T obj;
			ParseEvery(f, obj);
			t.push_back(obj);
		}
	}
	static void ParseEvery(const Value &val, int32_t &t) { t = val.GetInt(); }
	static void ParseEvery(const Value &val, int64_t &t) { t = val.GetInt64(); }
	static void ParseEvery(const Value &val, uint32_t &t) { t = val.GetUint(); }
	static void ParseEvery(const Value &val, uint64_t &t) { t = val.GetUint64(); }
	static void ParseEvery(const Value &val, double &t) { t = val.GetDouble(); }
	static void ParseEvery(const Value &val, bool &t) { t = val.GetBool(); }
	static void ParseEvery(const Value &val, std::string &t) { t = val.GetString(); }
	static void ParseEvery(const Value &val, char t[])
	{
		int size = ARRAY_SIZE(t);
		const char *s = val.GetString();
		int len = strlen(s);
		//int len = val.GetStringLength();
		strncpy(t, s, std::min(size, len));
	}
};

class Class
{
  private:
	//创建对象的函数指针定义
	MsgObj *(*factory)(void);
	//用来存储当前注册的所有可动态创建的Class
	static std::map<int, Class *> _classidmap;
	static std::map<std::string, Class *> _classnamemap;
	//Class类不允许外部初始化
	Class() {}
	int _classid;
	std::string _classname;

  public:
	template <class T>
	static Class *Register(int id, std::string name)
	{
		if ((_classidmap.count(id) == 0) && (_classnamemap.count(name) == 0))
		{
			Class *cls = new Class();
			cls->_classid = id;
			cls->factory = []() -> MsgObj * { return new T; };
			_classidmap[id] = cls; //在这里注册该class类
			_classnamemap[name] = cls;
		}
		return _classidmap[id];
	};
	int GetClassId() const { return _classid; }
	std::string GetClassName() const { return _classname; }
	MsgObj *NewInstance() const
	{
		return factory();
	}
	static Class *FindClass(int id)
	{
		if (_classidmap.count(id) != 0)
		{
			return _classidmap[id];
		}
		return NULL;
	}
	static Class *FindClass(std::string name)
	{
		if (_classnamemap.count(name) != 0)
		{
			return _classnamemap[name];
		}
		return NULL;
	}
};
std::map<int, Class *> Class::_classidmap = std::map<int, Class *>();
std::map<std::string, Class *> Class::_classnamemap = std::map<std::string, Class *>();
MsgObj *MsgObj::CreateObject(const std::string &json)
{
	Document dom;
	dom.Parse(json.data());
	
	if (dom.HasMember("ClassId"))
	{
		const Value &val = dom["ClassId"];
		Class *cl = Class::FindClass(val.GetInt());
		if (cl != NULL)
		{
			MsgObj *p = (MsgObj *)cl->NewInstance();
			p->ParseJson(dom);
			return p;
		}
	}
	return nullptr;
}

#endif