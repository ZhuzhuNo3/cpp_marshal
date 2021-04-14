#include "to_str.h"
#include <iostream>
using namespace std;

struct test {
    int a = 1;
    map<int, string> b = {{2,"2"}};
    string to_string(const string & tab = "") const {
        return
            string("\"a\": ")
            + std::to_string(a)
            + "," + (tab.size() == 0 ? " " : "\n")
            + string("\"b\": ")
            + TO_STR(b, tab);
    }
};

int main()
{
    vector<test> vv = {
        {}
    };
    map<string,char> x = {{"abc\ndef",'1'}};
    map<int,const char> y = {{2,'2'}};
    cout << TO_STR(vv) << endl << endl;
    cout << TO_STR(vv, "  ") << endl << endl;
    cout << TO_STR(x, "    ") << endl << endl;
    cout << TO_STR(y) << endl << endl;
}

