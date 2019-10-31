#include "poset.h"

#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <iostream>
#include <vector>
#include <algorithm>
    
#ifdef NDEBUG
  const bool debug{ false };
#else
  const bool debug{ true };
#endif

#define DEBUG(x) do {                                                          \
    using std::to_string;                                                      \
    if (debug) { std::cerr << x << std::endl; }                                \
} while (0)


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
		DEBUG("poset_new()");
		if(!freed_ids().empty()) {
			unsigned long new_id = *freed_ids().begin();
			freed_ids().erase(freed_ids().begin());
			
			DEBUG("poset_new: poset " + to_string(new_id) + " created");
			return new_id;
		}
		
		sets().push_back(poset());
		set_elem_ids().push_back(id_space());
		unsigned long new_id = (unsigned long)sets().size() - 1;
		DEBUG("poset_new: poset " + to_string(new_id) + " created");
		return new_id;
	}

	void poset_delete(unsigned long id) {
		DEBUG("poset_delete(" + to_string(id) + ")");
		if(exists(id) == false) {
			DEBUG("poset_delete: poset "+ to_string(id) + " does not exist");
			return;	
		}
		
		sets()[id].clear();
        set_elem_ids()[id].clear();
        
		freed_ids().insert(id);
		
		while(*freed_ids().begin() == sets()[id].size() + 1) {
			freed_ids().erase(freed_ids().begin());
			
            sets().pop_back();
            set_elem_ids().pop_back();
        }
        DEBUG("poset_delete: poset " + to_string(id) + " deleted");
	}

	size_t poset_size(unsigned long id) {
		DEBUG("poset_size("+ to_string(id) + ")");
		
		if(exists(id) == false) {
			DEBUG("poset_size: poset " + to_string(id) + " does not exist");
			return 0;	
		}
		size_t size = set_elem_ids()[id].size();
		
		DEBUG("poset_size: poset " + to_string(id) + 
				" contains " + to_string(size) + " element(s)");
		return size;
	}

	bool poset_insert(unsigned long id, char const *value) {
		DEBUG("poset_insert(" + to_string(id) + ", " + quote(repr(value)) + ")");
		
		if(exists(id) == false){
			DEBUG("poset_insert: poset " + to_string(id) + "does not exist");
			return false;
		}
		if(value == nullptr) {
			DEBUG("poset_insert: invalid value (NULL)");
			return false;
		}
		
		if(set_elem_ids()[id].count(value) > 0)
		{
			DEBUG("poset_insert: poset " + to_string(id) + 
					", element " + quote(repr(value)) +  " already exists");
			return false;
		}
		
		set_elem_ids()[id].insert(value);
		DEBUG("poset_insert: poset " + to_string(id) + 
				", element " + quote(repr(value)) + " inserted");
		return true;
	}

	bool poset_remove(unsigned long id, char const *value) {
		DEBUG("poset_remove(" + to_string(id) + ", " + quote(repr(value)) + ")");
		
		if(exists(id) == false){
			DEBUG("poset_remove: poset " + to_string(id) + "does not exist");
			return false;
		}
		if(value == nullptr) {
			DEBUG("poset_remove: invalid value (NULL)");
			return false;
		}
		if(set_elem_ids()[id].count(value) == 0){
			DEBUG("poset_remove: poset " + to_string(id) + 
					", element " + quote(repr(value)) + " does not exist");
			return false;
		}
		
		set_elem_ids()[id].erase(value);
		DEBUG("poset_remove: poset " + to_string(id) + 
				", element " + quote(repr(value)) + " removed");
		return true;
	}

	bool poset_add(unsigned long id, char const *value1, char const *value2) {
		DEBUG("poset_add(" + to_string(id) + ", " + 
				quote(repr(value1)) + ", " + quote(repr(value2)) + ")");
		
		if(invalid_params(id, value1, value2)) {
        	DEBUG("poset_add: poset " + to_string(id) + ", element " +
					quote(repr(value1)) + " or " + 
                    quote(repr(value2)) + " does not exist");
        	return false;
		}
        
        elem element1 = elem_of_value(value1, id);
        elem element2 = elem_of_value(value2, id);
        
        if(sets()[id][element1].count(element2) > 0 || 
           sets()[id][element2].count(element1) > 0) 
        {
        	DEBUG("poset_add: poset " + to_string(id) + ", relation (" + 
					quote(repr(value1)) + ", " + 
                    quote(repr(value2)) + ") cannot be added");
        	return false;
		}
        
		std::vector<elem> new_reachable = reachable_from (element2, sets()[id]);
		std::vector<elem> new_reaching = reachable_from (element1, reverse(sets()[id]));
        
		for(auto i = new_reaching.begin(); i != new_reaching.end(); i++)
			for(auto j = new_reachable.begin(); j != new_reachable.end(); j++)
				sets()[id][*i].insert(*j);
                
		DEBUG("poset_add: poset " + to_string(id) + ", relation (" + 
				quote(repr(value1)) + ", " + quote(repr(value2)) + ") added");
                
		return true;
	}

	bool poset_del(unsigned long id, char const *value1, char const *value2) {	
		DEBUG("poset_del(" + to_string(id) + ", " + quote(repr(value1)) + 
				", " + quote(repr(value2)) + ")");
        
		if(invalid_params(id, value1, value2)) {
        	DEBUG("poset_del: poset " + to_string(id) + ", element " + 
                quote(repr(value1)) + " or " + 
                quote(repr(value2)) + " does not exist");
            return false;
		}
        
        elem element1 = elem_of_value(value1, id);
        elem element2 = elem_of_value(value2, id);
		
        if(sets()[id][element1].count(element2) == 0){
        	DEBUG("poset_del: poset " + to_string(id) + ", relation (" + 
					quote(repr(value1)) + ", " + 
                    quote(repr(value2)) + ") cannot be deleted");
        	return false;
		}
		
		sets()[id][element1].erase(element2);
        
        // testing whether element1 -> element2 shouldn't still exist 
        // due to element1 still being transitively greater than element2
        std::vector<elem> still_reachable = reachable_from (element1, sets()[id]);
        
        if(std::find(still_reachable.begin(), still_reachable.end(), element2) 
                != still_reachable.end()) 
        {
            sets()[id][element1].insert(element2);
            DEBUG("poset_del: poset " + to_string(id) + ", relation (" + 
					quote(repr(value1)) + ", " + 
                    quote(repr(value2)) + ") cannot be deleted");
            return false;
        }
        
		DEBUG("poset_del: poset " + to_string(id) + ", relation (" + 
				quote(repr(value1)) + ", " + quote(repr(value2)) + ") deleted");
                
		return true;
	}

	bool poset_test(unsigned long id, char const *value1, char const *value2) {
		DEBUG("poset_test(" + to_string(id) + ", " + 
				quote(repr(value1)) + ", " + quote(repr(value2)) + ")");
		
		if(invalid_params(id, value1, value2)) {
        	DEBUG("poset_test poset " + to_string(id) + ", element " + 
                quote(repr(value1)) + " or " + 
                quote(repr(value2)) + " does not exist");
        	return false;
		}
		
        elem element1 = elem_of_value(value1, id);
        elem element2 = elem_of_value(value2, id);
        
		std::vector<elem> greater_than_value1 = reachable_from(element1, sets()[id]);
		bool result = std::find(greater_than_value1.begin(), 
                                greater_than_value1.end(), 
                                element2) != greater_than_value1.end();
						 
		if(result) {  
            DEBUG("poset_test: poset " + to_string(id) + ", relation (" + 
				quote(repr(value1)) + ", " + quote(repr(value2)) + ") exists");
		}
        else {
            DEBUG("poset_test: poset " + to_string(id) + ", relation (" + 
				quote(repr(value1)) + ", " + quote(repr(value2)) + ") does not exist");
		}
        
		return result;
	}

	void poset_clear(unsigned long id) {
		DEBUG("poset_clear(" + to_string(id) + ")");
		if(exists(id) == false){
			DEBUG("poset_clear: poset " + to_string(id) + " does not exist");
			return;
		}
		
		sets()[id].clear();
        set_elem_ids()[id].clear();
        DEBUG("poset_clear: poset " + to_string(id) + " cleared");
	}
}
