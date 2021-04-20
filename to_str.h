#ifndef TO_STR_H_
#define TO_STR_H_

#include <functional>
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
class CppSerialize
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
                s += CppSerialize<decltype(x)>(x, _tab).Str()
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
                s += CppSerialize<decltype(x.first)>(x.first, _tab).Str()
                    + ": "
                    + CppSerialize<decltype(x.second)>(x.second, _tab).Str()
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

    std::string _print_class(const std::vector<std::function<std::string(std::string)>> & mems) {
        std::string s;
        if (!mems.empty()) {
            for (auto m : mems) {
                s += m(_tab) + "," + (_tab.size() ? "\n" : " ");
            }
            s.pop_back();
            s.pop_back();
            _add_tab(s);
            if (_tab.size() != 0) {
                s = "\n" + s + "\n";
            }
        }
        return "{" + s + "}";
    }

    template<typename C> std::string _print(const C & x, decltype(&C::get_member) = nullptr)
    {
        return _print_class(x.get_member());
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
    CppSerialize(const T & t, const std::string & tab = "") : _tab(tab), _t(t) {}
    const T & _t;
    std::string Str() {
        return _print(_t);
    }
};

#define SIZE_OF_ARGS(...) SIZE_OF_ARGS_(0,##__VA_ARGS__,60,59,58,57,56,55,54,53,52,51,50,49,48,47,46,45,44,43,42,41,40,39,38,37,36,35,34,33,32,31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0)
#define SIZE_OF_ARGS_(I0,I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,I12,I13,I14,I15,I16,I17,I18,I19,I20,I21,I22,I23,I24,I25,I26,I27,I28,I29,I30,I31,I32,I33,I34,I35,I36,I37,I38,I39,I40,I41,I42,I43,I44,I45,I46,I47,I48,I49,I50,I51,I52,I53,I54,I55,I56,I57,I58,I59,I60,R,...) R

#define BUILD_MEM_IN_CLASS(Type, ...)                                 \
    std::vector<std::function<std::string(std::string)>>              \
    get_member() const {                                              \
        auto & t = *this;                                             \
        std::vector<std::function<std::string(std::string)>> members{ \
            BUILD_MEM_N(SIZE_OF_ARGS(__VA_ARGS__),##__VA_ARGS__)      \
        };                                                            \
        (void)&t;                                                     \
        return members;                                               \
    };

#define BUILD_MEM(Type, ...)                                                          \
    std::function<std::vector<std::function<std::string(std::string)>>(const Type &)> \
    get_member_of_##Type = [](const Type & t){                                        \
        std::vector<std::function<std::string(std::string)>> members{                 \
            BUILD_MEM_N(SIZE_OF_ARGS(__VA_ARGS__),##__VA_ARGS__)                      \
        };                                                                            \
        (void)&t;                                                                     \
        return members;                                                               \
    };                                                                                \
    template<>                                                                        \
    class CppSerialize<Type>                                                                 \
    {                                                                                 \
        void _add_tab(std::string & s) {                                              \
            if (_tab.size() != 0) {                                                   \
                s = _tab + s;                                                         \
                size_t i, pos = 0;                                                    \
                while ((i = s.find('\n', pos)) != std::string::npos) {                \
                    s = s.substr(0, i+1) + _tab + s.substr(i+1);                      \
                    pos = i + _tab.size();                                            \
                }                                                                     \
            }                                                                         \
        }                                                                             \
        std::string _print() {                                                        \
            std::string s;                                                            \
            const auto & mems = get_member_of_##Type(_t);                             \
            if (!mems.empty()) {                                                      \
                for (auto m : mems) {                                                 \
                    s += m(_tab) + "," + (_tab.size() ? "\n" : " ");                  \
                }                                                                     \
                s.pop_back();                                                         \
                s.pop_back();                                                         \
                _add_tab(s);                                                          \
                if (_tab.size() != 0) {                                               \
                    s = "\n" + s + "\n";                                              \
                }                                                                     \
            }                                                                         \
            return "{" + s + "}";                                                     \
        }                                                                             \
        std::string _tab;                                                             \
    public:                                                                           \
        CppSerialize(const Type & t, const std::string & tab = "") : _tab(tab), _t(t) {}     \
        const Type & _t;                                                              \
        std::string Str() {                                                           \
            return _print();                                                          \
        }                                                                             \
    };

#define BUILD_MEM_N(Count, ...) BUILD_MEM_N_(Count, ##__VA_ARGS__)
#define BUILD_MEM_N_(Count, ...) EXPAND_##Count(__VA_ARGS__)

#define GET_MEM(Member) [t](const std::string & Tab){ \
    return std::string("\""#Member"\": ") +           \
    CppSerialize<decltype(t.Member)>(t.Member,Tab).Str();    \
}

#define EXPAND_60(Member, ...) GET_MEM(Member), EXPAND_59(__VA_ARGS__)
#define EXPAND_59(Member, ...) GET_MEM(Member), EXPAND_58(__VA_ARGS__)
#define EXPAND_58(Member, ...) GET_MEM(Member), EXPAND_57(__VA_ARGS__)
#define EXPAND_57(Member, ...) GET_MEM(Member), EXPAND_56(__VA_ARGS__)
#define EXPAND_56(Member, ...) GET_MEM(Member), EXPAND_55(__VA_ARGS__)
#define EXPAND_55(Member, ...) GET_MEM(Member), EXPAND_54(__VA_ARGS__)
#define EXPAND_54(Member, ...) GET_MEM(Member), EXPAND_53(__VA_ARGS__)
#define EXPAND_53(Member, ...) GET_MEM(Member), EXPAND_52(__VA_ARGS__)
#define EXPAND_52(Member, ...) GET_MEM(Member), EXPAND_51(__VA_ARGS__)
#define EXPAND_51(Member, ...) GET_MEM(Member), EXPAND_50(__VA_ARGS__)
#define EXPAND_50(Member, ...) GET_MEM(Member), EXPAND_49(__VA_ARGS__)
#define EXPAND_49(Member, ...) GET_MEM(Member), EXPAND_48(__VA_ARGS__)
#define EXPAND_48(Member, ...) GET_MEM(Member), EXPAND_47(__VA_ARGS__)
#define EXPAND_47(Member, ...) GET_MEM(Member), EXPAND_46(__VA_ARGS__)
#define EXPAND_46(Member, ...) GET_MEM(Member), EXPAND_45(__VA_ARGS__)
#define EXPAND_45(Member, ...) GET_MEM(Member), EXPAND_44(__VA_ARGS__)
#define EXPAND_44(Member, ...) GET_MEM(Member), EXPAND_43(__VA_ARGS__)
#define EXPAND_43(Member, ...) GET_MEM(Member), EXPAND_42(__VA_ARGS__)
#define EXPAND_42(Member, ...) GET_MEM(Member), EXPAND_41(__VA_ARGS__)
#define EXPAND_41(Member, ...) GET_MEM(Member), EXPAND_40(__VA_ARGS__)
#define EXPAND_40(Member, ...) GET_MEM(Member), EXPAND_39(__VA_ARGS__)
#define EXPAND_39(Member, ...) GET_MEM(Member), EXPAND_38(__VA_ARGS__)
#define EXPAND_38(Member, ...) GET_MEM(Member), EXPAND_37(__VA_ARGS__)
#define EXPAND_37(Member, ...) GET_MEM(Member), EXPAND_36(__VA_ARGS__)
#define EXPAND_36(Member, ...) GET_MEM(Member), EXPAND_35(__VA_ARGS__)
#define EXPAND_35(Member, ...) GET_MEM(Member), EXPAND_34(__VA_ARGS__)
#define EXPAND_34(Member, ...) GET_MEM(Member), EXPAND_33(__VA_ARGS__)
#define EXPAND_33(Member, ...) GET_MEM(Member), EXPAND_32(__VA_ARGS__)
#define EXPAND_32(Member, ...) GET_MEM(Member), EXPAND_31(__VA_ARGS__)
#define EXPAND_31(Member, ...) GET_MEM(Member), EXPAND_30(__VA_ARGS__)
#define EXPAND_30(Member, ...) GET_MEM(Member), EXPAND_29(__VA_ARGS__)
#define EXPAND_29(Member, ...) GET_MEM(Member), EXPAND_28(__VA_ARGS__)
#define EXPAND_28(Member, ...) GET_MEM(Member), EXPAND_27(__VA_ARGS__)
#define EXPAND_27(Member, ...) GET_MEM(Member), EXPAND_26(__VA_ARGS__)
#define EXPAND_26(Member, ...) GET_MEM(Member), EXPAND_25(__VA_ARGS__)
#define EXPAND_25(Member, ...) GET_MEM(Member), EXPAND_24(__VA_ARGS__)
#define EXPAND_24(Member, ...) GET_MEM(Member), EXPAND_23(__VA_ARGS__)
#define EXPAND_23(Member, ...) GET_MEM(Member), EXPAND_22(__VA_ARGS__)
#define EXPAND_22(Member, ...) GET_MEM(Member), EXPAND_21(__VA_ARGS__)
#define EXPAND_21(Member, ...) GET_MEM(Member), EXPAND_20(__VA_ARGS__)
#define EXPAND_20(Member, ...) GET_MEM(Member), EXPAND_19(__VA_ARGS__)
#define EXPAND_19(Member, ...) GET_MEM(Member), EXPAND_18(__VA_ARGS__)
#define EXPAND_18(Member, ...) GET_MEM(Member), EXPAND_17(__VA_ARGS__)
#define EXPAND_17(Member, ...) GET_MEM(Member), EXPAND_16(__VA_ARGS__)
#define EXPAND_16(Member, ...) GET_MEM(Member), EXPAND_15(__VA_ARGS__)
#define EXPAND_15(Member, ...) GET_MEM(Member), EXPAND_14(__VA_ARGS__)
#define EXPAND_14(Member, ...) GET_MEM(Member), EXPAND_13(__VA_ARGS__)
#define EXPAND_13(Member, ...) GET_MEM(Member), EXPAND_12(__VA_ARGS__)
#define EXPAND_12(Member, ...) GET_MEM(Member), EXPAND_11(__VA_ARGS__)
#define EXPAND_11(Member, ...) GET_MEM(Member), EXPAND_10(__VA_ARGS__)
#define EXPAND_10(Member, ...) GET_MEM(Member), EXPAND_9(__VA_ARGS__)
#define EXPAND_9(Member, ...) GET_MEM(Member), EXPAND_8(__VA_ARGS__)
#define EXPAND_8(Member, ...) GET_MEM(Member), EXPAND_7(__VA_ARGS__)
#define EXPAND_7(Member, ...) GET_MEM(Member), EXPAND_6(__VA_ARGS__)
#define EXPAND_6(Member, ...) GET_MEM(Member), EXPAND_5(__VA_ARGS__)
#define EXPAND_5(Member, ...) GET_MEM(Member), EXPAND_4(__VA_ARGS__)
#define EXPAND_4(Member, ...) GET_MEM(Member), EXPAND_3(__VA_ARGS__)
#define EXPAND_3(Member, ...) GET_MEM(Member), EXPAND_2(__VA_ARGS__)
#define EXPAND_2(Member, ...) GET_MEM(Member), EXPAND_1(__VA_ARGS__)
#define EXPAND_1(Member, ...) GET_MEM(Member)
#define EXPAND_0(...)

#define TO_STR(x,args...) CppSerialize<decltype(x)>(x,##args).Str()

#endif /* TO_STR_H_ */
