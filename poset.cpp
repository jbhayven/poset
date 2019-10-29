#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <iostream>
#include <vector>
#include <algorithm>

namespace {
    using elem = const std::string*;
    using greater = std::unordered_set<elem>;

    using poset = std::unordered_map<elem, greater>;
    using id_space = std::unordered_set<std::string>;
    
    std::vector<poset>& sets() {
		static std::vector<poset> all_posets;
		return all_posets;
	} 
    
    std::vector<id_space>& set_elem_ids() {
		static std::vector<id_space> all_set_elem_ids;
		return all_set_elem_ids;
	} 
    
    // ids previously occupied but freed
    // the ultimate structure type is yet to be decided 
    std::unordered_set<unsigned long>& freed_ids() {
		static std::unordered_set<unsigned long> ids;
		return ids;
	}
    
    poset reverse(const poset& p) {
        poset reversed_poset;
        
        for(auto i = p.begin(); i != p.end(); i++) {
            elem lesser = i->first;
            const std::unordered_set<elem>& greater_elems = i->second;
            
            for(auto j = greater_elems.begin(); j != greater_elems.end(); j++) {
                reversed_poset[*j].insert(lesser);
            }
        }
        
        return reversed_poset;
    }

    std::vector<elem> reachable_from(elem const element, const poset& p) {
        std::unordered_set<elem> reached;
        std::vector<elem> candidates;
        
        reached.insert(element);
        candidates.push_back(element);
        
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
        if(freed_ids().count(id) > 0) return false;
        if(id >= sets().size()) return false;
        
        return true;
    }
    
    bool invalid_params(unsigned long id, 
                        char const *value1, 
                        char const *value2) 
    {
        if(exists(id) == false) return true;
    
        if(value1 == nullptr) return true;
        if(value2 == nullptr) return true;
   
        if(set_elem_ids()[id].count(value1) == 0) return true;
        if(set_elem_ids()[id].count(value2) == 0) return true;
    
        return false;
    }
    
    std::string repr(const char *word) {
        if(word == nullptr) return "NULL";
        else return word;
    }
    
    std::string quote(std::string word) {
        return "\"" + word + "\"";
    }
    
    elem elem_of_value(const char *value, unsigned long id) {
        if(set_elem_ids()[id].count(value) == 0) return nullptr;
        
        return &(*(set_elem_ids()[id].find(value))); 
    }
}

namespace jnp1 {
	unsigned long poset_new() {
		if(!freed_ids().empty()) {
			unsigned long new_id = *freed_ids().begin();
			freed_ids().erase(freed_ids().begin());
			
			return new_id;
		}
		
		sets().push_back(poset());
		set_elem_ids().push_back(id_space());
		
		return sets().size() - 1;
	}

	void poset_delete(unsigned long id) {
		if(exists(id) == false) return;
		
		sets()[id].clear();
        set_elem_ids()[id].clear();
        
		freed_ids().insert(id);
		
		while(*freed_ids().begin() == sets()[id].size() + 1) {
			freed_ids().erase(freed_ids().begin());
			
            sets().pop_back();
            set_elem_ids().pop_back();
        }
	}

	size_t poset_size(unsigned long id) {
		if(exists(id)) return 0;
		
		return set_elem_ids()[id].size();
	}

	bool poset_insert(unsigned long id, char const *value) {
		if(exists(id) == false) return false;
		if(value == nullptr) return false;
		
		if(set_elem_ids()[id].count(value) > 0) return false;
		
		set_elem_ids()[id].insert(value);
		return true;
	}

	bool poset_remove(unsigned long id, char const *value) {
		if(exists(id) == false) return false;
		if(value == nullptr) return false;
		
		if(set_elem_ids()[id].count(value) == 0) return false;
		
		set_elem_ids()[id].erase(value);
		return true;
	}

	bool poset_add(unsigned long id, char const *value1, char const *value2) {
		if(invalid_params(id, value1, value2)) return false;
        
        elem element1 = elem_of_value(value1, id);
        elem element2 = elem_of_value(value2, id);
        
        if(element1 == nullptr) return false;
        if(element2 == nullptr) return false;
        
        if(sets()[id][element1].count(element2) > 0) return false;
        
		std::vector<elem> new_reachable = reachable_from (element2, sets()[id]);
		std::vector<elem> new_reaching = reachable_from (element1, reverse(sets()[id]));
		
		for(auto i = new_reaching.begin(); i != new_reaching.end(); i++)
			for(auto j = new_reaching.begin(); j != new_reaching.end(); j++)
				sets()[id][*i].insert(*j);
		
		return true;
	}

	bool poset_del(unsigned long id, char const *value1, char const *value2) {
		if(invalid_params(id, value1, value2)) return false;
        
        elem element1 = elem_of_value(value1, id);
        elem element2 = elem_of_value(value2, id);
        
        if(element1 == nullptr) return false;
        if(element2 == nullptr) return false;
		
        if(sets()[id][element1].count(element2) == 0) return false;
		
		sets()[id][element1].erase(element2);
		
		std::vector<elem> now_unreachable = reachable_from (element2, sets()[id]);
		std::vector<elem> now_unreaching = reachable_from (element1, reverse(sets()[id]));
		
		for(auto i = now_unreaching.begin(); i != now_unreaching.end(); i++) {
			for(auto j = now_unreachable.begin(); j != now_unreachable.end(); j++) {
				if(sets()[id][*i].count(*j) > 0) {
					sets()[id][element1].insert(element2);
					return false;
				}
			}
		} 
		
		return true;
	}

	bool poset_test(unsigned long id, char const *value1, char const *value2) {
		if(invalid_params(id, value1, value2)) return false;
        
        elem element1 = elem_of_value(value1, id);
        elem element2 = elem_of_value(value2, id);
        
        if(element1 == nullptr) return false;
        if(element2 == nullptr) return false;
		
		std::vector<elem> greater_than_value1 = reachable_from(element1, sets()[id]);
		
		return std::find(greater_than_value1.begin(), greater_than_value1.end(), 
						 element2) != greater_than_value1.end();
	}

	void poset_clear(unsigned long id) {
		if(exists(id) == false) return;
		
		sets()[id].clear();
        set_elem_ids()[id].clear();
	}
}

int main() {    
    return 0;
}
