#ifndef __SYLAR_CONFIG_H__
#define __SYLAR_CONFIG_H__

#include <memory>                   // 智能指针
#include <sstream>                  // 系列化
#include <string>
#include <boost/lexical_cast.hpp>   // 内存转换  
#include "log.h"


namespace sylar
{

// 参数基类
class ConfigVarBase 
{
public:
    typedef std::shared_ptr<ConfigVarBase> ptr;
    ConfigVarBase(const std::string& name, const std::string& description = "") 
        :m_name(name)
        ,m_description(description) {

    }
    virtual ~ConfigVarBase() {}

    const std::string& getName() const { return m_name;}
    const std::string& getDescription() const { return m_description;}

    virtual std::string toString() = 0; // 转换为明文，调试或输出到文件
    virtual bool fromString(const std::string& val) = 0;    // 解析
protected:
    std::string m_name;
    std::string m_description;
};

// 具体实现类，基础类型得类
template<class T>
class ConfigVar : public ConfigVarBase
{
public:
    typedef std::shared_ptr<ConfigVar> ptr;
    
    ConfigVar(const std::string& name
            ,const T& default_value
            ,const std::string& description = "")
        :ConfigVarBase(name, description)
        ,m_val(default_value) {

    }

    // 把东西转为string，即转为明文，以便调试或输出到文件
    std::string toString() override {
        try {
            return boost::lexical_cast<std::string>(m_val);
        } catch (std::exception& e) {
            SYLAR_LOG_ERROR(SYLAR_LOG_ROOT()) << "ConfigVar::toString exception"
                << e.what() << " convert: " << typeid(m_val).name() << " to string";
        }
        return "";
    }

    bool fromString(const std::string& val) override {
        try {
            m_val = boost::lexical_cast<T>(val);
        } catch (std::exception& e) {
            SYLAR_LOG_ERROR(SYLAR_LOG_ROOT()) << "ConfigVar::toString exception"
                << e.what() << " convert: string to" << typeid(m_val).name();
        }
        return false;
    }

    const T getValue() const { return m_val;}
    void setValue(const T& v) { m_val = v;}
private:
    T m_val;
};

// 管理的类
class Config
{
public:
    typedef std::map<std::string, ConfigVarBase::ptr> ConfigVarMap;

    // 定义类：功能：定义的时候就可以给他赋值
    template<class T>
    static typename ConfigVar<T>::ptr Lookup(const std::string& name,
            const T& default_value, const std::string& description = "") 
    {
        auto tmp = Lookup<T>(name);
        // 定义初始化的时候看能不能找到name，找到了直接就返回
        if (tmp) {
            SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "Looup name=" << name << " exists";
            return tmp;
        }  

        // 没有名字，或名字不在规定范围内
        if (name.find_first_not_of("abcdefghijlmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ._0123456789") 
                != std::string::npos) {
            SYLAR_LOG_ERROR(SYLAR_LOG_ROOT()) << "Lookup name invalid " << name;
            throw std::invalid_argument(name);
        }

        typename ConfigVar<T>::ptr v(new ConfigVar<T>(name, default_value, description));
        s_datas[name] = v;
        return v;
    }

    // 查找类
    template<class T>
    static typename ConfigVar<T>::ptr Lookup(const std::string& name) 
    {
        auto it = s_datas.find(name);
        if (it == s_datas.end()) {
            return nullptr; // 没找到
        }
        // 找到了还需要先转为智能指针
        return std::dynamic_pointer_cast<ConfigVar<T>>(it->second); 
    
    }
private:
    static ConfigVarMap s_datas;
};


    
} // namespace sylar


#endif // !__SYLAR_CONFIG_H__