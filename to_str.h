#ifndef TO_STR_H_
#define TO_STR_H_

#include <string>
#include <map>
#include <unordered_map>
#include <vector>
#include <list>
#include <forward_list>
#include <set>
#include <unordered_set>
#include <deque>

template<typename T>
class Print
{
    void _add_tab(std::string & s) {
        if (_tab.size() != 0) {
            s = _tab + s;
            size_t i, pos = 0;
            while ((i = s.find('\n', pos)) != std::string::npos) {
                s = s.substr(0, i+1) + _tab + s.substr(i+1);
                pos = i + _tab.size();
            }
        }
    }

    template<typename, template<typename...> class Ref>
        struct is_specialization : std::false_type {};

    template<template<typename...> class Ref, typename... Args>
        struct is_specialization<Ref<Args...>, Ref> : std::true_type {};

    template<typename C> std::string _print_arr(const C & a) {
        std::string s;
        bool addTab = _tab.size();
        if (a.size() > 0) {
            for (auto x : a) {
                s += Print<decltype(x)>(x, _tab).Str()
                    + ","
                    + (addTab ? "\n" : " ");
            }
            s.pop_back();
            s.pop_back();
            _add_tab(s);
        }
        if (_tab.size() != 0) {
            s = "\n" + s + "\n";
        }
        return "[" + s + "]";
    }

    template<typename C> std::string _print_map(const C & m) {
        std::string s;
        bool addTab = _tab.size();
        if (m.size() > 0) {
            for (auto x : m) {
                s += Print<decltype(x.first)>(x.first, _tab).Str()
                    + ": "
                    + Print<decltype(x.second)>(x.second, _tab).Str()
                    + ","
                    + (addTab ? "\n" : " ");
            }
            s.pop_back();
            s.pop_back();
            _add_tab(s);
        }
        if (addTab) {
            s = "\n" + s + "\n";
        }
        return "{" + s + "}";
    }

    // need struct has function `std::string to_string(const std::string & tab = "") const;`
    template<typename C> std::string _print(const C & x, decltype(&C::to_string) = nullptr)
    {
        std::string s = x.to_string(_tab);
        if (s.size() != 0) {
            _add_tab(s);
        }
        if (_tab.size() != 0) {
            s = "\n" + s + "\n";
        }
        return "{" + s + "}";
    }

    template<typename C> std::string _print(const C & x, typename std::enable_if<
                                                   std::is_same<C, std::string>::value
                                                   || std::is_same<C, const std::string>::value
                                                   >::type* = nullptr)
    { return "\""+x+"\""; }

    template<typename C> std::string _print(const C & x, typename std::enable_if<
                                                   std::is_arithmetic<C>::value
                                                   >::type* = nullptr)
    {
        if (std::is_same<C, char>::value) {
            return "'" + std::string(1,x) + "'";
        }
        return std::to_string(x);
    }

    template<typename C> std::string _print(const C & v, typename std::enable_if<
                                                   is_specialization<C, std::vector>::value
                                                   || is_specialization<C, std::list>::value
                                                   || is_specialization<C, std::forward_list>::value
                                                   || is_specialization<C, std::set>::value
                                                   || is_specialization<C, std::unordered_set>::value
                                                   || is_specialization<C, std::multiset>::value
                                                   || is_specialization<C, std::unordered_multiset>::value
                                                   || is_specialization<C, std::deque>::value
                                                   >::type* = nullptr)
    { return _print_arr(v); }

    template<typename C> std::string _print(const C & m, typename std::enable_if<
                                                   is_specialization<C, std::map>::value
                                                   || is_specialization<C, std::unordered_map>::value
                                                   || is_specialization<C, std::multimap>::value
                                                   || is_specialization<C, std::unordered_multimap>::value
                                                   >::type* = nullptr)
    { return _print_map(m); }

    std::string _tab;
public:
    Print(const T & t, const std::string & tab = "") : _t(t), _tab(tab) {}
    const T & _t;
    std::string Str() {
        return _print(_t);
    }
};

#define TO_STR(x,args...) Print<decltype(x)>(x,##args).Str()

#endif /* TO_STR_H_ */
