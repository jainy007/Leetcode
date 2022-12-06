import unittest
from soln.sol14 import Solution
from inputs import *

ANS_14 = [
    "",    
    "",
    "fl",
    "ab",
    "a",
    "",
    "a",
    "c"
]
class TEST_14(unittest.TestCase):
    def test_14(self):
        k = Solution()
        for i in range(len(input14)):
            print(k.longestCommonPrefix(input14[i]))
            self.assertEqual(k.longestCommonPrefix(input14[i]), ANS_14[i])

if __name__ == '__main__':
    unittest.main()