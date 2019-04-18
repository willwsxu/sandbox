//#include <string>
#include <bits/stringfwd.h>
#include "ttstl/string.h"
#include <iostream>
#include <sstream>

using namespace std;
using namespace tt;

void PrintInfo(const String& s)
{
    static int n = 0;
    cout << "====================" << endl;
    cout << "n: " << ++n << endl;
    cout << "obj: " << (void*)&s << endl;
    cout << "data: " << (void*)s.data() << endl;
    cout << "size: " << s.size() << endl;
    cout << "capacity: " << s.capacity() << endl;
    cout << "is_bound: " << s.is_bound() << endl;
    cout << "value: '" << s << "'" << endl;
    cout << "c_str: '" << s.c_str() << "'" << endl;
    cout << "====================" << endl << endl;
}

void Simple()
{
    String e;
    PrintInfo(e);

    String s("foo");
    PrintInfo(s);

    s.append(" bar");
    PrintInfo(s);

    s.clear();
    PrintInfo(s);

    s.bind("baz");
    PrintInfo(s);

    s.clear();
    PrintInfo(s);

    s.append("12345678901234567890123");
    PrintInfo(s);

    s.append("456");
    PrintInfo(s);

    s.shrink_to_fit();
    PrintInfo(s);

    s.assign("1234567890");
    PrintInfo(s);

    s.shrink_to_fit();
    PrintInfo(s);

    s.erase();
    PrintInfo(s);
}

void Stream()
{
    String s;
    stringstream ss;
    ss << "foobar\n";
    ss >> s;
    PrintInfo(s);
}

void Replace()
{
    String str("The quick brown fox jumps over the lazy dog.");
    str.replace(10, 5, "red");
    PrintInfo(str);
    str.replace(str.cbegin(), str.cbegin() + 3, 1, 'A');
    PrintInfo(str);
    str.replace(20, 1, "ed");
    PrintInfo(str);
    str.replace(0, 0, "Yesterday, ");
    PrintInfo(str);
    str.resize(str.length()-1);
    PrintInfo(str);
    str.replace(str.cend(), str.cend(), "!");
    PrintInfo(str);
}

int main()
{
    Simple();
    Stream();
    Replace();

    return 0;
}
