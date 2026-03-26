<script setup lang="ts">
import { Shield, LayoutDashboard, ScrollText } from 'lucide-vue-next'
import { ref, onMounted, watch } from 'vue'
import api from '@/api/axios'
import L4TCPRuleForm from '@/components/L4TCPForm.vue'
import L2RuleForm from './L2RuleForm.vue'
import L3RuleForm from './L3RuleForm.vue'
import L4SimpleRuleForm from './L4SimpleRuleForm.vue'

import type { FirewallRule, RuleLayer } from '@/types/firewall'

type SubmitEvent = {
  action: number
  payload: Record<string, unknown>
}

const selectedLayer = ref<RuleLayer>('L4TCP')
const selectedPermit = ref(0)
const selectedStorage = ref<0 | 1>(1) // 1 = paměť, 0 = databáze

const rules = ref<FirewallRule[]>([])
const isLoading = ref(false)
const error = ref<string | null>(null)
const submitMessage = ref<string | null>(null)
const submitError = ref<string | null>(null)
const isDeletingAll = ref(false)

const fetchRules = async () => {
  isLoading.value = true
  error.value = null

  try {
    const response = await api.get(`/fireWall/selectRule/${selectedPermit.value}`, {
      params: {
        ID: 1,
        layer: selectedLayer.value,
        fromMemory: selectedStorage.value,
      },
    })

    rules.value = response.data
  } catch (e: any) {
    error.value =
      e.response?.status != null
        ? `Chyba serveru: ${e.response.status}`
        : 'Nepodařilo se načíst pravidla.'
  } finally {
    isLoading.value = false
  }
}

const submitRule = async ({ action, payload }: SubmitEvent) => {
  submitMessage.value = null
  submitError.value = null

  try {
    await api.post(`/fireWall/createRule/${action}`, payload)
    submitMessage.value = 'Pravidlo bylo úspěšně odesláno.'
    selectedPermit.value = action
    await fetchRules()
  } catch (e: any) {
    if (e.response) {
      submitError.value = `Chyba při odeslání: ${e.response.status}`
    } else {
      submitError.value = 'Nepodařilo se odeslat pravidlo na server.'
    }
  }
}

const deleteRule = async (id: number, fromMemory = selectedStorage.value) => {
  submitMessage.value = null
  submitError.value = null

  try {
    await api.delete('/fireWall/deleteRule', {
      params: {
        ID: id,
        layer: selectedLayer.value,
        fromMemory,
      },
    })

    submitMessage.value =
      fromMemory === 1
        ? `Pravidlo ${id} bylo smazáno z paměti.`
        : `Pravidlo ${id} bylo smazáno z databáze.`

    await fetchRules()
  } catch (e: any) {
    submitError.value =
      e.response?.status != null
        ? `Chyba při mazání: ${e.response.status}`
        : 'Nepodařilo se smazat pravidlo.'
  }
}

const deleteAllRulesFromDatabase = async () => {
  submitMessage.value = null
  submitError.value = null

  if (rules.value.length === 0) {
    submitError.value = 'Nejsou žádná databázová pravidla ke smazání.'
    return
  }

  const confirmed = window.confirm(
    `Opravdu chceš smazat všech ${rules.value.length} pravidel z databáze?`
  )

  if (!confirmed) return

  isDeletingAll.value = true

  try {
    for (const rule of rules.value) {
      await api.delete('/fireWall/deleteRule', {
        params: {
          ID: rule.ID,
          layer: selectedLayer.value,
          fromMemory: 0,
        },
      })
    }

    submitMessage.value = 'Všechna pravidla byla smazána z databáze.'
    await fetchRules()
  } catch (e: any) {
    submitError.value =
      e.response?.status != null
        ? `Chyba při mazání všech pravidel: ${e.response.status}`
        : 'Nepodařilo se smazat všechna pravidla z databáze.'
  } finally {
    isDeletingAll.value = false
  }
}

const formatAction = (rule: FirewallRule) => {
  const permit = (rule as any)?.data?.permit
  return permit === true ? 'ALLOW' : 'DENY'
}

const formatAddress = (address?: [string, number]) => {
  if (Array.isArray(address) && address.length >= 2) {
    return `${address[0]}/${address[1]}`
  }
  return '-'
}

const formatRuleDetail = (rule: FirewallRule) => {
  const data = (rule as any).data

  if (!data) return '-'

  if (rule.ruleType === 'L2') {
    return `${data.sourceMAC ?? '-'} → ${data.destMAC ?? '-'}`
  }

  if (rule.ruleType === 'L3') {
    return `${formatAddress(data.source)} → ${formatAddress(data.dest)}`
  }

  if (rule.ruleType === 'L4Simple') {
    return `${data.sourcePort ?? '-'} → ${data.destPort ?? '-'}`
  }

  if (rule.ruleType === 'L4TCP') {
    return `SP: ${data.sourcePort ?? '-'}, DP: ${data.destPort ?? '-'}, Flags: ${data.tcpFlags ?? data.flags ?? '-'}`
  }

  return '-'
}

watch([selectedLayer, selectedStorage], () => {
  fetchRules()
})

onMounted(fetchRules)
</script>

<template>
  <div class="min-h-screen bg-background text-foreground flex">
    <aside class="w-64 shrink-0 bg-sidebar text-sidebar-foreground border-r border-sidebar-border p-4">
      <div class="flex items-center gap-2 px-2 pb-4 font-semibold">
        <Shield :size="20" />
        <span>PacketBlocker</span>
      </div>

      <nav class="mt-2 flex flex-col gap-1">
        <router-link
          to="/"
          class="flex items-center gap-2 px-3 py-2 rounded-lg text-sm no-underline
                 text-sidebar-foreground/70 hover:text-sidebar-foreground hover:bg-sidebar-accent"
          active-class="bg-sidebar-accent text-sidebar-foreground ring-1 ring-sidebar-ring/40"
          exact-active-class="bg-sidebar-accent text-sidebar-foreground ring-1 ring-sidebar-ring/40"
        >
          <LayoutDashboard :size="18" />
          Dashboard
        </router-link>

        <router-link
          to="/rules"
          class="flex items-center gap-2 px-3 py-2 rounded-lg text-sm no-underline
                 text-sidebar-foreground/70 hover:text-sidebar-foreground hover:bg-sidebar-accent"
          active-class="bg-sidebar-accent text-sidebar-foreground ring-1 ring-sidebar-ring/40"
        >
          <Shield :size="18" />
          Firewall Rules
        </router-link>

        <router-link
          to="/logs"
          class="flex items-center gap-2 px-3 py-2 rounded-lg text-sm no-underline
                 text-sidebar-foreground/70 hover:text-sidebar-foreground hover:bg-sidebar-accent"
          active-class="bg-sidebar-accent text-sidebar-foreground ring-1 ring-sidebar-ring/40"
        >
          <ScrollText :size="18" />
          Real-time Trafic
        </router-link>
      </nav>
    </aside>

    <div class="flex-1 flex flex-col">
      <header class="h-14 bg-card border-b border-border flex items-center justify-between px-4">
        <h1 class="text-lg font-semibold">Firewall Rules</h1>

        <div
          class="inline-flex items-center gap-2 px-3 py-2 rounded-full text-xs font-semibold
                 bg-green-500/10 text-green-600 border border-green-500/20"
        >
          <span class="w-2 h-2 rounded-full bg-green-500"></span>
          System Active
        </div>
      </header>

      <main class="p-6 space-y-6">
        <section class="rounded-xl border border-border bg-card p-6">
          <h2 class="text-base font-semibold mb-4">Vytvořit pravidlo</h2>

          <div class="grid grid-cols-1 md:grid-cols-2 gap-4 mb-6">
            <div>
              <label class="block text-sm mb-1">Typ pravidla</label>
              <select
                v-model="selectedLayer"
                class="w-full rounded-lg border border-border bg-background px-3 py-2"
              >
                <option value="L2">L2</option>
                <option value="L3">L3</option>
                <option value="L4Simple">L4Simple</option>
                <option value="L4TCP">L4TCP</option>
              </select>
            </div>

            <div>
              <label class="block text-sm mb-1">Zdroj pravidel</label>
              <select
                v-model="selectedStorage"
                class="w-full rounded-lg border border-border bg-background px-3 py-2"
              >
                <option :value="1">Paměť</option>
                <option :value="0">Databáze</option>
              </select>
            </div>
          </div>

          <L2RuleForm
            v-if="selectedLayer === 'L2'"
            @submit="submitRule"
          />

          <L3RuleForm
            v-else-if="selectedLayer === 'L3'"
            @submit="submitRule"
          />

          <L4SimpleRuleForm
            v-else-if="selectedLayer === 'L4Simple'"
            @submit="submitRule"
          />

          <L4TCPRuleForm
            v-else-if="selectedLayer === 'L4TCP'"
            @submit="submitRule"
          />

          <div class="mt-4 flex flex-wrap items-center gap-3">
            <button
              v-if="selectedStorage === 0"
              @click="deleteAllRulesFromDatabase"
              :disabled="isDeletingAll || rules.length === 0"
              class="rounded-lg px-4 py-2 text-sm font-medium border border-red-500/20 text-red-500 hover:bg-red-500/10 disabled:opacity-50"
            >
              {{ isDeletingAll ? 'Mažu...' : 'Smazat všechna pravidla z databáze' }}
            </button>

            <span v-if="submitMessage" class="text-sm text-green-600">
              {{ submitMessage }}
            </span>

            <span v-if="submitError" class="text-sm text-red-500">
              {{ submitError }}
            </span>
          </div>
        </section>

        <div v-if="isLoading" class="text-sm text-muted-foreground">
          Načítám pravidla...
        </div>

        <div v-else-if="error" class="text-sm text-red-500">
          {{ error }}
        </div>

        <div
          v-else
          class="overflow-x-auto rounded-xl border border-border bg-card"
        >
          <table class="w-full text-sm">
            <thead class="bg-muted/40 border-b border-border">
              <tr>
                <th class="text-left px-4 py-3">ID</th>
                <th class="text-left px-4 py-3">Typ pravidla</th>
                <th class="text-left px-4 py-3">Akce</th>
                <th class="text-left px-4 py-3">Detail</th>
                <th class="text-left px-4 py-3">Save</th>
                <th class="text-left px-4 py-3">Update</th>
                <th class="text-left px-4 py-3">Mazání</th>
              </tr>
            </thead>

            <tbody>
              <tr
                v-for="rule in rules"
                :key="rule.ID"
                class="border-b border-border last:border-b-0"
              >
                <td class="px-4 py-3">{{ rule.ID }}</td>
                <td class="px-4 py-3">{{ rule.ruleType }}</td>
                <td class="px-4 py-3">{{ formatAction(rule) }}</td>
                <td class="px-4 py-3">{{ formatRuleDetail(rule) }}</td>
                <td class="px-4 py-3">{{ rule.save ? 'true' : 'false' }}</td>
                <td class="px-4 py-3">{{ rule.update ? 'true' : 'false' }}</td>
                <td class="px-4 py-3">
                  <button
                    @click="deleteRule(rule.ID)"
                    class="rounded-lg px-3 py-1 text-sm border border-red-500/20 text-red-500 hover:bg-red-500/10"
                  >
                    Smazat
                  </button>
                </td>
              </tr>

              <tr v-if="rules.length === 0">
                <td colspan="7" class="px-4 py-6 text-center text-muted-foreground">
                  Žádná pravidla nebyla nalezena.
                </td>
              </tr>
            </tbody>
          </table>
        </div>
      </main>
    </div>
  </div>
</template>