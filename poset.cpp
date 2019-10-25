#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <iostream>
#include <vector>

namespace {
    using elem = std::string;
    using greater = std::vector<elem>;

    using poset = std::unordered_map<elem, greater>;
    
    std::vector<poset> sets; 
    
    // ids previously occupied but freed
    // the ultimate structure type is yet to be decided 
    std::unordered_set<unsigned long> freed_ids; 

    poset reverse(const poset& p);
    std::vector<elem> reachable_from(char const *value, const poset& p);


    poset reverse(const poset& p) {
        poset reversed_poset;
        
        for(auto i = p.begin(); i != p.end(); i++) {
            elem lesser = (*i).first;
            const std::vector<elem>& greater_elems = (*i).second;
            
            for(auto j = greater_elems.begin(); j != greater_elems.end(); j++) {
                reversed_poset[*j].push_back(lesser);
            }
        }
        
        return reversed_poset;
    }

    std::vector<elem> reachable_from(char const *value, const poset& p) {
        std::unordered_set<elem> reached;
        std::vector<elem> candidates;
        
        reached.insert(std::string(value));
        candidates.push_back(std::string(value));
        
        for(size_t i = 0; i < candidates.size(); i++) {            
            if(p.count(candidates[i]) == 0) continue; 
            const std::vector<elem>& greater_elems = p.at(candidates[i]);
            
            for(auto i = greater_elems.begin(); i != greater_elems.end(); i++) {
                if(reached.count(*i) == 0) {
                    reached.insert(*i);
                    candidates.push_back(*i);
                }
            }
        }
        
        return candidates;
    }
}

int main() {
    poset settt;
    char const a[] = "HEHHEE";
    char const b[] = "LOLO";
    char const c[] = "PFFF";
    
    using std::cout;
    using std::endl;
    
    using std::string;
    
    settt[string(a)].push_back(string(b));
    settt[string(a)].push_back(string(c));
    
    cout << "Added" << endl;
    
    std::vector reached = reachable_from(a, settt);
    cout << "Reachable: ";
    for(auto i = reached.begin(); i != reached.end(); i++) {
        cout << (*i) << ", ";
    }
    cout << endl;
    
    poset reversed = reverse(settt);
    cout << "Reversed: " << endl;
    for(auto i = reversed.begin(); i != reversed.end(); i++) {
        cout << (*i).first << ": ";
        for(auto j = (*i).second.begin(); j != (*i).second.end(); j++) {
            cout << (*j) << ", ";
        }
        cout << endl;
    }
    
    return 0;
}
