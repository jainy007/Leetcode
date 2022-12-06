class Solution(object):
    def longestCommonPrefix(self, strs):
        """
        :type strs: List[str]
        :rtype: str
        """
        def Sorting(strs):
            lst2 = sorted(strs, key=len)
            return lst2

        # print(Sorting(strs))
        strs = Sorting(strs)

        soln_stack = strs[0]
        soln_stack = strs[0]
        

        def check(string1, string2):
            ans = ""
            for x, y in zip(string1,string2):
                if x == y:
                    ans = ans + y
                else:
                    break
            # print("Returned ", ans)
            return ans

        for i in range(len(strs)):
            soln_stack = check(strs[i], soln_stack)
        return soln_stack