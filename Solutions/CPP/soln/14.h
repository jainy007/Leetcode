#include <vector>
#include <string>
#include <algorithm>

class Solution {
    public:
    
    static std::string longestCommonPrefix(std::vector<std::string>& strs) {
        int m = strs.size();
        if (m==1){
            return strs[0];  
        }
        std::sort(strs.begin(), strs.end(), [](std::string a, std::string b) {return a.length() < b.length(); });
        std::string common = strs[0];
        
        // cout<<common<<"------"<<endl;

        int i = 0;
        int j = 0;
        int x = 0;
        for (i=0;i<m;i++)
            // cout<<strs[i]<<endl;
        for (i=0;i<m;i++){
            std::string sol;
            for (j=0;j<strs[0].length();j++){
                // cout<<"comparing "<<strs[i][j]<<" of "<<strs[i]<<" with "<<common[j]<<" of "<<common<<" i = "<<i<<" j = "<<j<<endl;
                if((j<common.length()) && (strs[i][j]!=common[j])){
                    // cout<<"ERASING "<<endl;
                    for (int k = 0; k<j; k++){
                        sol.push_back(common[k]);
                    }
                    // cout<<"SOL "<<sol<<endl;
                    common = sol;
                    
                }
            }
        }
    return common;
    }

};
