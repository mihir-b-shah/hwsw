
#include "cache.h"
#include "code_info.h"

#include <unordered_map>

using namespace std;

/* architectural metadata */

// training unit (maps pc to last address)
unordered_map<uint64_t, uint64_t> tu;

// mapping cache (maps address to next address)
unordered_map<uint64_t, uint64_t> cache;
unordered_map<uint64_t, uint64_t> context; // instr_ids for cache update

/* performance metadata */
unordered_map<uint64_t, uint64_t> outstanding;
unordered_map<uint64_t, uint32_t> issued;
unordered_map<uint64_t, uint32_t> untimely;
unordered_map<uint64_t, uint32_t> accurate;
unordered_map<uint64_t, uint32_t> total;
uint64_t divergence = 0;

/* get most specific prediction possible */
uint64_t get_prediction(uint64_t pc, uint64_t addr) {
    if (cache.find(addr) != cache.end()) {
        return cache[addr];
    } else {
        return 0;
    }
}

void l2c_notify_useful(uint64_t addr, uint64_t pc) {
    accurate[pc]++;
}

void sisb_prefetcher_initialize() {

}

void sisb_prefetcher_operate(uint64_t addr, uint64_t pc, uint8_t cache_hit, uint8_t type, uint32_t degree, vector<uint64_t>& prefetch_candidates, uint64_t instr_id) 
{
    assert(prefetch_candidates.size() == 0);
    // only consider L2 demand misses
    if (type != LOAD)
        return;

    // initialize values
    if (issued.find(pc) == issued.end()) {
        issued[pc] = 0;
        untimely[pc] = 0;
        accurate[pc] = 0;
        total[pc] = 0;
    }
    total[pc]++;

    uint64_t addr_B = addr >> LOG2_BLOCK_SIZE;

    /* training */
    if (tu.find(pc) != tu.end()) {
        uint64_t last_addr = tu[pc];

        bool divergent = cache.find(last_addr) != cache.end() && cache[last_addr] != addr_B;
        bool convergent = cache.find(last_addr) != cache.end() && cache[last_addr] == addr_B;

        code_informer::get_instance()->accept_query(CALL_STACK, instr_id, {context[last_addr]},
          [pc, divergent, convergent](std::vector<void*>& results){
            std::fprintf(stderr, "EVENT: %s PC: %" PRIxPTR " BEFCTXT: %s AFTCTXT: %s\n",
              divergent ? "divrg" : convergent ? "convg" : "fresh", pc, static_cast<char*>(results[1]), 
              static_cast<char*>(results[0]));
          });
        if (divergent) {
            divergence++;
        }
        cache[last_addr] = addr_B;
        context[last_addr] = instr_id;
    }
    tu[pc] = addr_B;

    /* prediction */
    uint64_t pred = get_prediction(pc, addr_B);
    uint32_t count = 0;
    while (count < degree && pred != 0) {
        // issue prefetch
        pred <<= LOG2_BLOCK_SIZE;
        //int was_issued = parent->prefetch_line(pc, addr, pred, FILL_LLC, 0);
        prefetch_candidates.push_back(pred);
        pred >>= LOG2_BLOCK_SIZE;
        //if (was_issued) {
            outstanding[pred] = pc;
            issued[pc]++;
            count++;
        //}
        
        // get next prediction
        pred = get_prediction(pc, pred);
    }
}

void sisb_prefetcher_cache_fill(uint64_t addr, uint32_t set, uint32_t way, uint8_t prefetch, uint64_t evicted_addr) {
    uint64_t addr_B = addr >> LOG2_BLOCK_SIZE;
    if (outstanding.find(addr_B) != outstanding.end()) {
        uint64_t last_pc = outstanding[addr_B];
        if (prefetch) {
        } else {
            untimely[last_pc]++;    
        }
        outstanding.erase(addr_B);
    }
}

bool flag = false;
void sisb_prefetcher_final_stats() {
    // only print stats once
    if (flag)
        return;
    flag = true;

    cout << "performance stats:" << endl;
    uint32_t mappings = cache.size();
    cout << "mapping cache size: " << (mappings) << endl;
    cout << "divergence: " << divergence << endl;
}