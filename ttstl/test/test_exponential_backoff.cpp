#include <iostream>
#include "ttstl/exponential_backoff.h"

using namespace std;

int main()
{
    tt::ExponentialBackoff<> eb1(std::chrono::milliseconds(1));

    cout << "---- eb1 ----" << endl;
    for (size_t i(0); i<66; ++i) {
        cout << eb1.Next().count() << endl;
    }

    tt::ExponentialBackoff<> eb2(std::chrono::milliseconds(1), std::chrono::seconds(1));

    cout << "---- eb2 ----" << endl;
    for (size_t i(0); i<16; ++i) {
        cout << eb2.Next().count() << endl;
    }

    tt::ExponentialBackoff<> eb3(std::chrono::milliseconds(1), std::chrono::seconds(1), 0);

    cout << "---- eb3 ----" << endl;
    for (size_t i(0); i<16; ++i) {
        cout << eb3.Next().count() << endl;
    }

    tt::ExponentialBackoff<> eb4(std::chrono::milliseconds(1), std::chrono::milliseconds::max(), 0);

    cout << "---- eb4 ----" << endl;
    for (size_t i(0); i<66; ++i) {
        cout << eb4.Next().count() << endl;
    }

    return 0;
}
