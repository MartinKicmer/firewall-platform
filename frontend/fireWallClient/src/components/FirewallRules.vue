<script setup lang="ts">
import { Shield, LayoutDashboard, Search, ScrollText, Plus, Database, Cpu, Trash2, Save } from 'lucide-vue-next'
import { ref, onMounted, watch } from 'vue'
import api from '@/api/axios'
import L4TCPRuleForm from '@/components/L4TCPForm.vue'
import L2RuleForm from './L2RuleForm.vue'
import L3RuleForm from './L3RuleForm.vue'
import L4SimpleRuleForm from './L4SimpleRuleForm.vue'
import type { FirewallRule, RuleLayer } from '@/types/firewall'

const selectedLayer = ref<RuleLayer>('L4TCP')
const selectedStorage = ref<0 | 1>(1)
const rules = ref<FirewallRule[]>([])
const submitMessage = ref<string | null>(null)

const fetchRules = async () => {
  try {
    const res = await api.get(`/fireWall/selectRule/0`, { params: { ID: 1, layer: selectedLayer.value, fromMemory: selectedStorage.value } });
    rules.value = res.data;
  } catch (e) { }
}

const deleteRule = async (id: number) => {
  if (!confirm(`Smazat pravidlo ID ${id}?`)) return
  try {
    await api.delete('/fireWall/deleteRule', { params: { ID: id, layer: selectedLayer.value, fromMemory: selectedStorage.value } });
    fetchRules();
  } catch (e) { }
}

const submitRule = async ({ action, payload }: any) => {
  try {
    await api.post(`/fireWall/createRule/${action}`, payload);
    submitMessage.value = "Uloženo"; setTimeout(() => submitMessage.value = null, 3000);
    fetchRules();
  } catch (e) { }
}

watch([selectedLayer, selectedStorage], () => fetchRules())
onMounted(fetchRules)
</script>

<template>
  <div class="layout-wrapper">
    <aside class="sidebar">
      <div class="flex items-center gap-2.5 px-2 pb-6 pt-2 font-semibold text-lg text-white">
        <Shield :size="22" class="text-blue-500" /> <span class="tracking-tight font-bold">PacketBlocker</span>
      </div>
      <nav class="flex flex-col gap-1.5">
        <router-link to="/" class="group nav-link"><LayoutDashboard :size="19" /> Dashboard</router-link>
        <router-link to="/rules" class="group nav-link nav-link-active"><Shield :size="19" class="text-blue-500" /> Firewall Rules</router-link>
        <router-link to="/search" class="group nav-link"><Search :size="19"/> Search Rules</router-link>
        <router-link to="/logs" class="group nav-link"><ScrollText :size="19" /> Real-time Traffic</router-link>
      </nav>
    </aside>

    <div class="main-container">
      <header class="header-nav">
        <div class="flex items-center gap-3"><Shield :size="20" class="text-blue-500" /><h1 class="text-xl font-extrabold tracking-tighter text-white">Firewall Rules</h1></div>
        <div v-if="submitMessage" class="text-xs font-bold bg-green-500/10 text-green-400 px-4 py-2 rounded-lg border border-green-500/20 animate-bounce">{{ submitMessage }}</div>
      </header>

      <main class="p-8 space-y-8">
        <section class="stats-card stats-card-accent">
          <div class="flex items-center gap-2 mb-6"><Plus :size="18" class="text-blue-500" /><h2 class="text-base font-bold text- tracking-tight">New Rule</h2></div>
          <div class="grid grid-cols-1 md:grid-cols-2 gap-6 mb-8 p-4 bg-pb-dark rounded-xl border border-pb-border">
            <div class="space-y-2">
              <label class="text-[10px] font-bold text-pb-text-dim uppercase tracking-widest flex items-center gap-2"><Cpu :size="14" /> Layer</label>
              <select v-model="selectedLayer" class="input-base">
                <option value="L2">L2 (MAC)</option><option value="L3">L3 (IP)</option><option value="L4Simple">L4 Simple</option><option value="L4TCP">L4 TCP</option>
              </select>
            </div>
            <div class="space-y-2">
              <label class="text-[10px] font-bold text-pb-text-dim uppercase tracking-widest flex items-center gap-2"><Database :size="14" /> Storage</label>
              <select v-model="selectedStorage" class="input-base"><option :value="1">Memory</option><option :value="0">Database</option></select>
            </div>
          </div>
          <div class="bg-[#1c1c1f] p-6 rounded-xl border border-pb-border shadow-inner">
              <L2RuleForm v-if="selectedLayer === 'L2'" @submit="submitRule" />
              <L3RuleForm v-else-if="selectedLayer === 'L3'" @submit="submitRule" />
              <L4SimpleRuleForm v-else-if="selectedLayer === 'L4Simple'" @submit="submitRule" />
              <L4TCPRuleForm v-else-if="selectedLayer === 'L4TCP'" @submit="submitRule" />
          </div>
        </section>

        <section class="stats-card !p-0 overflow-hidden">
            <div class="p-5 border-b border-pb-border bg-[#1c1c1f]/50 flex items-center justify-between px-8">
                <h2 class="text-sm font-bold text-white uppercase tracking-widest flex items-center gap-2"><Database :size="16" class="text-blue-500" /> Active Policy</h2>
                <span class="text-[10px] font-mono text-pb-text-dim">{{ rules.length }} rules</span>
            </div>
            <div class="table-container">
                <table class="data-table">
                    <thead><tr class="table-header"><th class="px-8 py-4">ID</th><th class="px-8 py-4">Typ</th><th class="px-8 py-4">Action</th><th class="px-8 py-4">State</th><th class="px-8 py-4 text-right">Delete</th></tr></thead>
                    <tbody class="divide-y divide-pb-border">
                        <tr v-for="rule in rules" :key="rule.ID" class="group table-row">
                            <td class="px-8 py-4 font-mono text-blue-400 text-xs">#{{ rule.ID }}</td>
                            <td class="px-8 py-4 text-xs font-bold">{{ rule.ruleType }}</td>
                            <td class="px-8 py-4"><span :class="rule.data?.permit ? 'text-green-400 bg-green-500/10' : 'text-red-400 bg-red-500/10'" class="badge border-current">{{ rule.data?.permit ? 'ALLOW' : 'DENY' }}</span></td>
                            <td class="px-8 py-4"><span class="flex items-center gap-2 text-xs text-pb-text-dim"><Save :size="12" :class="rule.save ? 'text-blue-500' : 'text-pb-hover'" /> {{ rule.save ? 'Perm' : 'Temp' }}</span></td>
                            <td class="px-8 py-4 text-right">
                                <button @click="deleteRule(rule.ID)" class="p-2 rounded-lg bg-red-500/5 text-red-400 hover:bg-red-500 hover:text-white transition-all border border-red-500/10"><Trash2 :size="14" /></button>
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