import unittest
from soln import sol1
from soln import sol14
from inputs import *
from ans import *


class TEST_1(unittest.TestCase):
    def test_1(self):
        p = sol1.Solution1()
        for i in range(len(input1)):
            self.assertEqual(p.twoSum(input1[i][0], input1[i][1]), ANS_1[i])

class TEST_14(unittest.TestCase):
    def test_14(self):
        k = sol14.Solution14()
        for i in range(len(input14)):
            print(k.longestCommonPrefix(input14[i]))
            self.assertEqual(k.longestCommonPrefix(input14[i]), ANS_14[i])

if __name__ == '__main__':
    unittest.main()