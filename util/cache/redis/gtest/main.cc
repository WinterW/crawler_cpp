
#include "gtest/gtest.h"

using ::testing::InitGoogleTest;
using ::testing::Test;
using ::testing::TestCase;
using ::testing::TestEventListeners;
using ::testing::TestInfo;
using ::testing::TestPartResult;
using ::testing::UnitTest;



int main(int argc, char *argv[])
{
  InitGoogleTest(&argc, argv);
  testing::GTEST_FLAG(catch_exceptions) = 1;
  RUN_ALL_TESTS();
  return 0;
}
