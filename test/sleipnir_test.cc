#include "sleipnir_math.h"

#include "gtest/gtest.h"

// TODO: make real benchmarks, this is just stub to build.

namespace sleipnir {
namespace {

TEST(SleipnirTest, Sleipner) {
    // Arrange
    int actual = spAdd(5, 5);

    // Assert
    EXPECT_EQ(actual, 10);
}

}  // namespace
}  // namespace sleipnir