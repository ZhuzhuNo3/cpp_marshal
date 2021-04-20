#include "to_str.h"
#include <iostream>
using namespace std;

// use BUILD_MEM_IN_CLASS or BUILD_MEM

struct test {
    int a = 1;
    map<int, string> b = {{2,"2"},{3,"3"}};
    BUILD_MEM_IN_CLASS(test,a,b);
};

// BUILD_MEM(test,a,b);

int main()
{
    vector<test> vv = {{}};
    cout << TO_STR(vv) << endl << endl;
    cout << TO_STR(vv,"  ") << endl << endl;
}

