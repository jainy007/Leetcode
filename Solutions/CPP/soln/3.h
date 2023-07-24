#include <string>
#include <unordered_set>

//DOCSTRING: SLIDING_WINDOW
/*
DESCRIPTION:
Given a string s, find the length of the longest substring without repeating characters.


*Example 1:

Input: s = "abcabcbb"
Output: 3
Explanation: The answer is "abc", with the length of 3.

*Example 2:

Input: s = "bbbbb"
Output: 1
Explanation: The answer is "b", with the length of 1.

*Example 3:

Input: s = "pwwkew"
Output: 3
Explanation: The answer is "wke", with the length of 3.
Notice that the answer must be a substring, "pwke" is a subsequence and not a substring.
*/

class Solution3 {
public:
    int lengthOfLongestSubstring(std::string s) {
        int i = 0;
        int j = 0;
        int maxlen = 0;
        std::unordered_set<char> seen;
        if (s.size()==0)
            return 0;

        while (i<s.size()){
            auto it=seen.find(s[i]);
            if(it == seen.end()){
                seen.insert(s[i]);
                if (maxlen < i-j+1)
                    maxlen = i-j+1;  
                i++;    
            } else {
                seen.erase(s[j]);
                j++;
            }
        }
        return maxlen;
    }
};