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
    template<typename, template<typename...> class Ref>
        struct is_specialization : std::false_type {};

    template<template<typename...> class Ref, typename... Args>
        struct is_specialization<Ref<Args...>, Ref> : std::true_type {};

    template<typename C> static std::string _print_arr(const C & a) {
        std::string s;
        if (a.size() > 0) {
            for (auto x : a) {
                s += Print<decltype(x)>(x).Str() + ",";
            }
            s.pop_back();
        }
        return "[" + s + "]";
    }

    template<typename C> static std::string _print_map(const C & m) {
        std::string s;
        if (m.size() > 0) {
            for (auto x : m) {
                s += Print<decltype(x.first)>(x.first).Str()
                    + ":"
                    + Print<decltype(x.second)>(x.second).Str()
                    + ",";
            }
            s.pop_back();
        }
        return "{" + s + "}";
    }

    // need struct has function `std::string to_string() const;`
    template<typename C> static std::string _print(const C & x, decltype(&C::to_string) = nullptr)
    { return "{" + x.to_string() + "}"; }

    template<typename C> static std::string _print(const C & x, typename std::enable_if<
                                                   std::is_same<C, std::string>::value
                                                   || std::is_same<C, const std::string>::value
                                                   >::type* = nullptr)
    { return "\""+x+"\""; }

    template<typename C> static std::string _print(const C & x, typename std::enable_if<
                                                   std::is_arithmetic<C>::value
                                                   >::type* = nullptr)
    {
        if (std::is_same<C, char>::value) {
            return "'" + std::string(1,x) + "'";
        }
        return std::to_string(x);
    }

    template<typename C> static std::string _print(const C & v, typename std::enable_if<
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

    template<typename C> static std::string _print(const C & m, typename std::enable_if<
                                                   is_specialization<C, std::map>::value
                                                   || is_specialization<C, std::unordered_map>::value
                                                   || is_specialization<C, std::multimap>::value
                                                   || is_specialization<C, std::unordered_multimap>::value
                                                   >::type* = nullptr)
    { return _print_map(m); }

public:
    Print(const T & t) : _t(t) {}
    const T & _t;
    std::string Str() { return _print<T>(_t); }
};

#define TO_STR(x) Print<decltype(x)>(x).Str()

#endif /* TO_STR_H_ */
