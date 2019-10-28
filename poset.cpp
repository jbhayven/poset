#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <iostream>
#include <vector>
#include <algorithm>

namespace {
    using elem = std::string;
    using greater = std::unordered_set<elem>;

    using poset = std::unordered_map<elem, greater>;
    
    std::vector<poset> sets; 
    
    // ids previously occupied but freed
    // the ultimate structure type is yet to be decided 
    std::unordered_set<unsigned long> freed_ids; 

    poset reverse(const poset& p);
    std::vector<elem> reachable_from(char const *value, const poset& p);


    poset reverse(const poset& p) {
        poset reversed_poset;
        
        for(auto i = p.begin(); i != p.end(); i++)
            reversed_poset.insert(make_pair(i->first, greater()));
        
        for(auto i = p.begin(); i != p.end(); i++) {
            elem lesser = i->first;
            const std::unordered_set<elem>& greater_elems = i->second;
            
            for(auto j = greater_elems.begin(); j != greater_elems.end(); j++) {
                reversed_poset[*j].insert(lesser);
            }
        }
        
        return reversed_poset;
    }

    std::vector<elem> reachable_from(char const *value, const poset& p) {
        std::unordered_set<elem> reached;
        std::vector<elem> candidates;
        
        reached.insert(value);
        candidates.push_back(value);
        
        for(size_t i = 0; i < candidates.size(); i++) {            
            if(p.count(candidates[i]) == 0) continue; 
            const std::unordered_set<elem>& greater_elems = p.at(candidates[i]);
            
            for(auto i = greater_elems.begin(); i != greater_elems.end(); i++) {
                if(reached.count(*i) == 0) {
                    reached.insert(*i);
                    candidates.push_back(*i);
                }
            }
        }
        
        return candidates;
    }
    
    bool exists(unsigned long id) {
        if(freed_ids.count(id) > 0) return false;
        if(id >= sets.size()) return false;
        
        return true;
    }
    
    bool invalid_params(unsigned long id, 
                        char const *value1, 
                        char const *value2) 
    {
        if(exists(id) == false) return true;
    
        if(value1 == nullptr) return true;
        if(value2 == nullptr) return true;
   
        if(sets[id].count(value1) == 0) return true;
        if(sets[id].count(value2) == 0) return true;
    
        return false;
    }
    
    std::string repr(const char *word) {
        if(word == nullptr) return "NULL";
        else return word;
    }
    
    std::string quote(std::string word) {
        return "\"" + word + "\"";
    }
}

unsigned long poset_new() {
    if(!freed_ids.empty()) {
        unsigned long new_id = *freed_ids.begin();
        freed_ids.erase(freed_ids.begin());
        
        return new_id;
    }
    
    sets.push_back(poset());
    
    return sets.size() - 1;
}

void poset_delete(unsigned long id) {
    if(exists(id) == false) return;
    
    sets[id].clear();
    freed_ids.insert(id);
    
    while(*freed_ids.begin() == sets[id].size() + 1) {
        freed_ids.erase(freed_ids.begin());
        sets.pop_back();
    }
}

size_t poset_size(unsigned long id) {
    if(exists(id)) return 0;
    
    return sets[id].size();
}

bool poset_insert(unsigned long id, char const *value) {
    if(exists(id) == false) return false;
    if(value == nullptr) return false;
    
    if(sets[id].count(value) > 0) return false;
    
    sets[id].insert(make_pair(value, greater()));
    return true;
}

bool poset_remove(unsigned long id, char const *value) {
    if(exists(id) == false) return false;
    if(value == nullptr) return false;
    
    if(sets[id].count(value) == 0) return false;
    
    sets[id].erase(value);
    return true;
}

bool poset_add(unsigned long id, char const *value1, char const *value2) {
    if(invalid_params(id, value1, value2)) return false;
        
    if(sets[id][value1].count(value2) != 0) return false;
    
    std::vector<elem> new_reachable = reachable_from (value2, sets[id]);
    std::vector<elem> new_reaching = reachable_from (value1, reverse(sets[id]));
    
    for(auto i = new_reaching.begin(); i != new_reaching.end(); i++)
        for(auto j = new_reaching.begin(); j != new_reaching.end(); j++)
            sets[id][*i].insert(*j);
    
    return true;
}

bool poset_del(unsigned long id, char const *value1, char const *value2) {
    if(invalid_params(id, value1, value2)) return false;
    
    if(sets[id][value1].count(value2) == 0) return false;
    
    sets[id][value1].erase(value2);
    
    std::vector<elem> now_unreachable = reachable_from (value2, sets[id]);
    std::vector<elem> now_unreaching = reachable_from (value1, reverse(sets[id]));
    
    for(auto i = now_unreaching.begin(); i != now_unreaching.end(); i++) {
        for(auto j = now_unreachable.begin(); j != now_unreachable.end(); j++) {
            if(sets[id][*i].count(*j) > 0) {
                sets[id][value1].insert(value2);
                return false;
            }
        }
    } 
    
    return true;
}

bool poset_test(unsigned long id, char const *value1, char const *value2) {
    if(invalid_params(id, value1, value2)) return false;
    
    std::vector<elem> greater_than_value1 = reachable_from(value1, sets[id]);
    
    return std::find(greater_than_value1.begin(), greater_than_value1.end(), 
                     value2) != greater_than_value1.end();
}

void poset_clear(unsigned long id) {
    if(exists(id) == false) return;
    
    sets[id].clear();
}

int main() {
    poset settt;
    char const a[] = "HEHHEE";
    char const b[] = "LOLO";
    char const c[] = "PFFF";
    
    using std::cout;
    using std::endl;
    
    using std::string;
    
    settt[string(a)].insert(string(b));
    settt[string(a)].insert(string(c));
    
    cout << "Added" << endl;
    
    std::vector<elem> reached = reachable_from(a, settt);
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
