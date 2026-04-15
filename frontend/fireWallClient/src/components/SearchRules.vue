<script setup lang="ts">
import { Shield, LayoutDashboard, ScrollText, Search, Database, Cpu, Trash2, Filter, ArrowUpDown } from 'lucide-vue-next'
import { ref, computed, onMounted } from 'vue'
import api from '@/api/axios'

const selectedLayer = ref('ALL')
const selectedStorage = ref<0 | 1>(1)
const rules = ref<any[]>([])
const loading = ref(false)
const sortOrder = ref<'asc' | 'desc'>('asc')


const layerWeight: Record<string, number> = { 'L2': 1, 'L3': 2, 'L4Simple': 3, 'L4TCP': 4 }

const fetchRules = async () => {
  loading.value = true
  rules.value = []
  
  const layersToFetch = selectedLayer.value === 'ALL' 
    ? ['L2', 'L3', 'L4Simple', 'L4TCP'] 
    : [selectedLayer.value]

  try {
    const requests = layersToFetch.map(layer => 
      api.get(`/fireWall/selectRule/0`, { 
        params: { ID: 1, layer: layer, fromMemory: selectedStorage.value } 
      }).then(res => {
        return res.data
          .filter((r: any) => selectedLayer.value === 'ALL' || r.ruleType?.includes(layer))
          .map((r: any) => ({ ...r, ruleLayer: layer }));
      })
    )

    const results = await Promise.all(requests)
    const allFetched = results.flat()

    const uniqueMap = new Map();
    allFetched.forEach(item => uniqueMap.set(item.ID, item));
    
    rules.value = Array.from(uniqueMap.values());
  } catch (e) {
    console.error("Chyba vyhledávání", e)
  } finally {
    loading.value = false
  }
}

const sortedRules = computed(() => {
  const sorted = [...rules.value].sort((a, b) => {
    const weightA = layerWeight[a.ruleLayer] || 99
    const weightB = layerWeight[b.ruleLayer] || 99
    
    if (sortOrder.value === 'asc') return weightA - weightB
    return weightB - weightA
  })
  return sorted
})

const toggleSort = () => {
  sortOrder.value = sortOrder.value === 'asc' ? 'desc' : 'asc'
}

const deleteRule = async (id: number, layer: string) => {
  if (!confirm(`Smazat pravidlo ${id}?`)) return
  try {
    await api.delete('/fireWall/deleteRule', { 
      params: { ID: id, layer: layer, fromMemory: selectedStorage.value } 
    });
    fetchRules()
  } catch (e) {}
}

onMounted(fetchRules)
</script>

<template>
  <div class="layout-wrapper">
    <aside class="sidebar">
      <div class="flex items-center gap-2.5 px-2 pb-6 pt-2 font-semibold text-lg text-white">
        <Shield :size="22" class="text-blue-500" /> <span class="tracking-tight font-bold">PacketBlocker</span>
      </div>
      <nav class="flex flex-col gap-1.5 flex-grow">
        <router-link to="/" class="group nav-link"><LayoutDashboard :size="19" /> Dashboard</router-link>
        <router-link to="/rules" class="group nav-link"><Shield :size="19" /> Firewall Rules</router-link>
        <router-link to="/search" class="group nav-link nav-link-active"><Search :size="19" class="text-blue-500" /> Search Rules</router-link>
        <router-link to="/logs" class="group nav-link"><ScrollText :size="19" /> Real-time Traffic</router-link>
      </nav>
    </aside>

    <div class="main-container">
      <header class="header-nav">
        <div class="flex items-center gap-3 font-black tracking-tighter text-white">
          <Search class="text-blue-500" /> Search Rules
        </div>
      </header>

      <main class="p-8 space-y-8">
        <section class="stats-card stats-card-accent">
          <div class="grid grid-cols-1 md:grid-cols-3 gap-6">
            <div class="space-y-2">
              <label class="text-[10px] font-bold text-pb-text-dim uppercase tracking-widest flex items-center gap-2"><Cpu :size="14" /> Layer</label>
              <select v-model="selectedLayer" class="input-base w-full bg-pb-dark border-pb-border text-white rounded-xl p-3 outline-none focus:border-blue-500 transition-all">
                <option value="ALL">All Layers</option>
                <option value="L2">L2 (MAC)</option>
                <option value="L3">L3 (IP)</option>
                <option value="L4Simple">L4 Simple</option>
                <option value="L4TCP">L4 TCP</option>
              </select>
            </div>

            <div class="space-y-2">
              <label class="text-[10px] font-bold text-pb-text-dim uppercase tracking-widest flex items-center gap-2"><Database :size="14" /> Storage</label>
              <select v-model="selectedStorage" class="input-base w-full bg-pb-dark border-pb-border text-white rounded-xl p-3 outline-none focus:border-blue-500 transition-all">
                <option :value="1">Memory</option>
                <option :value="0">Database</option>
              </select>
            </div>

            <div class="flex items-end">
              <button @click="fetchRules" :disabled="loading" class="w-full bg-blue-600 hover:bg-blue-500 text-white font-bold py-3 rounded-xl transition-all flex items-center justify-center gap-2">
                <Search :size="18" /> {{ loading ? 'Loading...' : 'Search' }}
              </button>
            </div>
          </div>
        </section>

        <section class="stats-card !p-0 overflow-hidden">
            <div class="table-container">
                <table class="data-table w-full text-left">
                    <thead>
                        <tr class="table-header bg-[#1c1c1f]/50">
                            <th @click="toggleSort" class="px-8 py-4 text-[10px] uppercase text-pb-text-dim tracking-widest cursor-pointer hover:text-white transition-colors group">
                                <div class="flex items-center gap-2">
                                    Layer 
                                    <ArrowUpDown :size="12" :class="sortOrder === 'asc' ? 'text-blue-500' : 'text-zinc-600'" />
                                </div>
                            </th>
                            <th class="px-8 py-4 text-[10px] uppercase text-pb-text-dim tracking-widest">ID</th>
                            <th class="px-8 py-4 text-[10px] uppercase text-pb-text-dim tracking-widest">Type</th>
                            <th class="px-8 py-4 text-[10px] uppercase text-pb-text-dim tracking-widest">Action</th>
                            <th class="px-8 py-4 text-[10px] uppercase text-pb-text-dim tracking-widest">Details</th>
                            <th class="px-8 py-4 text-right"></th>
                        </tr>
                    </thead>
                    <tbody class="divide-y divide-pb-border">
                        <tr v-for="rule in sortedRules" :key="rule.ruleLayer + rule.ID" class="group hover:bg-white/[0.02] transition-colors">
                            <td class="px-8 py-4">
                                <span :class="{
                                  'text-blue-400 bg-blue-500/10 border-blue-500/20': rule.ruleLayer === 'L2',
                                  'text-green-400 bg-green-500/10 border-green-500/20': rule.ruleLayer === 'L3',
                                  'text-purple-400 bg-purple-500/10 border-purple-500/20': rule.ruleLayer.includes('L4')
                                }" class="px-2 py-0.5 rounded text-[10px] font-black border uppercase">
                                    {{ rule.ruleLayer }}
                                </span>
                            </td>
                            <td class="px-8 py-4 font-mono text-blue-400 text-xs">#{{ rule.ID }}</td>
                            <td class="px-8 py-4 text-xs font-bold text-white">{{ rule.ruleType }}</td>
                            <td class="px-8 py-4 text-xs">
                                <span :class="rule.data?.permit ? 'text-green-400' : 'text-red-400'" class="font-bold">
                                    {{ rule.data?.permit ? 'ALLOW' : 'DENY' }}
                                </span>
                            </td>
                            <td class="px-8 py-4 text-[11px] text-pb-text-dim font-mono">
                                {{ rule.data?.sip || rule.data?.smac || '*' }} → {{ rule.data?.dip || rule.data?.dmac || '*' }}
                            </td>
                            <td class="px-8 py-4 text-right">
                                <button @click="deleteRule(rule.ID, rule.ruleLayer)" class="p-2 text-red-400 hover:bg-red-500/10 rounded-lg">
                                    <Trash2 :size="14" />
                                </button>
                            </td>
                        </tr>
                    </tbody>
                </table>
            </div>
        </section>
      </main>
    </div>
  </div>
</template>