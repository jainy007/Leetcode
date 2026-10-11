class Solution1(object):
    def twoSum(self, nums, target):
        """
        :type nums: List[int]
        :type target: int
        :rtype: List[int]
        """
        h_map = {} #val:index
        for i, val in enumerate(nums):
            diff = target-val
            if diff in h_map:
                return[h_map[diff], i]
            else:
                h_map[val] = i
        return