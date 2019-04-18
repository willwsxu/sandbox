#include <stdexcept>
#include <iostream>
#include <ttstl/vector_set.h>

using namespace tt;
using namespace std;

class TestFailure : public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;
};

#define ASSERT_EQ(a, b) \
    if ((a) != (b)) { throw TestFailure("Assertion Failure: " #a " != " #b); }

void RunTests()
{
    VectorSet<int> s{4, 8, 1, 3, 1};
    ASSERT_EQ(s.size(), 4);
    ASSERT_EQ(s.count(8), 1);
    ASSERT_EQ(s.count(1), 1);
    ASSERT_EQ(s.count(7), 0);
    ASSERT_EQ(s.find(4) - s.begin(), 2);
    ASSERT_EQ(s.find(1) - s.begin(), 0);
    ASSERT_EQ(s.find(7), s.end());

    s.erase(8);
    ASSERT_EQ(s.size(), 3);

    s.clear();
    ASSERT_EQ(s.empty(), true);
    ASSERT_EQ(s.size(), 0);

    s.insert(4);
    s.insert(1);
    s.insert(6);
    ASSERT_EQ(s.size(), 3);
    ASSERT_EQ(*s.begin(), 1);
    ASSERT_EQ(s, (VectorSet<int>{1, 4, 6}));
}

int main()
{
    try {
        RunTests();
    } catch (const TestFailure& e) {
        cout << "Test failed: " << e.what() << endl;
        return 1;
    }

    cout << "All tests passed" << endl;

    return 0;
}
