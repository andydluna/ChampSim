#include "hybrid_lru.h"

hybrid_lru::hybrid_lru(CACHE* cache) :
    hybrid_lru(cache, cache->NUM_SET, cache->NUM_WAY) {}

hybrid_lru::hybrid_lru(CACHE* cache, long sets, long ways) :
    replacement(cache),
    NUM_WAY(ways),
    last_used_cycles(sets, std::vector<uint64_t>(ways, 0)),
    access_frequencies(sets, std::vector<uint64_t>(ways, 0)) {}

long hybrid_lru::find_victim(uint32_t triggering_cpu, uint64_t instr_id, long set, const champsim::cache_block* current_set, champsim::address ip,
                      champsim::address full_addr, access_type type)
{
    long victim = 0;
    uint64_t min_cycle = last_used_cycles[set][0];
    uint64_t min_frequency = access_frequencies[set][0];
    
    for (long way = 1; way < NUM_WAY; way++) {
        if (access_frequencies[set][way] < min_frequency ||
            (access_frequencies[set][way] == min_frequency && last_used_cycles[set][way] < min_cycle)) {
            victim = way;
            min_cycle = last_used_cycles[set][way];
            min_frequency = access_frequencies[set][way];
        }
    }
    
    return victim;
}

void hybrid_lru::replacement_cache_fill(uint32_t triggering_cpu, long set, long way, champsim::address full_addr, champsim::address ip, champsim::address victim_addr,
                                 access_type type)
{
    last_used_cycles[set][way] = cycle;
    access_frequencies[set][way] = 1;
    
}

void hybrid_lru::update_replacement_state(uint32_t triggering_cpu, long set, long way, champsim::address full_addr, champsim::address ip,
                                   champsim::address victim_addr, access_type type, uint8_t hit)
{
    if (access_type{type} != access_type::WRITE) {
        if (hit) {
            access_frequencies[set][way]++;
        }
        last_used_cycles[set][way] = cycle++;
    }
}
