<script setup lang="ts">
import { Shield, LayoutDashboard, ScrollText } from 'lucide-vue-next'
import { ref, onMounted } from 'vue'
import api from '@/api/axios'

type RuleData = {
  permit?: boolean
  source?: [string, number]
  dest?: [string, number]
  destPort?: number
  sourcePort?: number
  protocol?: string
  ttlMin?: number
  ttlMax?: number
  tos?: number
  allowFrag?: boolean
  limitCount?: number
  tcpFlags?: number
  flags?: number
  minWindowsize?: number
  maxWindowsize?: number
  minWin?: number
  maxWin?: number
}

type FirewallRule = {
  ID: number
  ruleType: string
  save?: boolean
  update?: boolean
  data?: RuleData
}

type CreateL4TCPRulePayload = {
  ID: number
  layer: 'L4TCP'
  save: boolean
  sourcePort: number
  destPort: number
  flags: number
  minWin: number
  maxWin: number
  limitCount: number
}

const data = ref<FirewallRule[]>([])
const isLoading = ref(false)
const error = ref<string | null>(null)

const isSubmitting = ref(false)
const submitMessage = ref<string | null>(null)
const submitError = ref<string | null>(null)

const form = ref({
  ID: 1,
  permit: false,
  sourcePort: 444,
  destPort: -1,
  flags: 0,
  minWin: -1,
  maxWin: -1,
  limitCount: 1,
  save: false,
})

const buildL4TCPPayload = (): CreateL4TCPRulePayload => {
  return {
    ID: form.value.ID,
    layer: 'L4TCP',
    save: form.value.save,
    sourcePort: form.value.sourcePort,
    destPort: form.value.destPort,
    flags: form.value.flags,
    minWin: form.value.minWin,
    maxWin: form.value.maxWin,
    limitCount: form.value.limitCount,
  }
}

const fetchRules = async () => {
  isLoading.value = true
  error.value = null

  try {
    const action = form.value.permit ? 1 : 0

    const response = await api.get(`/fireWall/selectRule/${action}`, {
      params: {
        ID: form.value.ID,
        layer: 'L4TCP',
        fromMemory: 1,
      },
    })

    data.value = response.data
    console.log('Firewall rules:', response.data)
  } catch (e: any) {
    error.value =
      e.response?.status != null
        ? `Chyba serveru: ${e.response.status}`
        : 'Nelze se spojit s bránou. Zkontrolujte připojení k síti.'

    console.error('Network error:', e)
  } finally {
    isLoading.value = false
  }
}

const submitRule = async () => {
  isSubmitting.value = true
  submitMessage.value = null
  submitError.value = null

  const payload = buildL4TCPPayload()
  const action = form.value.permit ? 1 : 0

  try {
    const response = await api.post(`/fireWall/createRule/${action}`, payload)

    console.log('POST payload:', payload)
    console.log('POST response:', response.data)

    submitMessage.value = 'L4TCP pravidlo bylo úspěšně odesláno na server.'
    await fetchRules()
  } catch (e: any) {
    const status = e.response?.status
    const responseData = e.response?.data

    submitError.value = status
      ? `Chyba při odeslání: ${status}${responseData ? ` ${typeof responseData === 'string' ? responseData : JSON.stringify(responseData)}` : ''}`
      : 'Nepodařilo se odeslat pravidlo na server.'

    console.error('Submit error:', e)
  } finally {
    isSubmitting.value = false
  }
}

const formatAction = (rule: FirewallRule) => {
  return rule.data?.permit === true ? 'ALLOW' : 'DENY'
}

const formatFlags = (rule: FirewallRule) => {
  return rule.data?.tcpFlags ?? rule.data?.flags ?? '-'
}

const formatWindow = (rule: FirewallRule) => {
  const min = rule.data?.minWindowsize ?? rule.data?.minWin ?? -1
  const max = rule.data?.maxWindowsize ?? rule.data?.maxWin ?? -1
  return `${min} / ${max}`
}

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
          <h2 class="text-base font-semibold mb-4">Vytvořit L4TCP pravidlo</h2>

          <div class="grid grid-cols-1 md:grid-cols-2 gap-4">
            <div>
              <label class="block text-sm mb-1">ID</label>
              <input
                v-model.number="form.ID"
                type="number"
                class="w-full rounded-lg border border-border bg-background px-3 py-2"
              />
            </div>

            <div>
              <label class="block text-sm mb-1">Akce</label>
              <select
                v-model="form.permit"
                class="w-full rounded-lg border border-border bg-background px-3 py-2"
              >
                <option :value="true">ALLOW</option>
                <option :value="false">DENY</option>
              </select>
            </div>

            <div>
              <label class="block text-sm mb-1">Source port</label>
              <input
                v-model.number="form.sourcePort"
                type="number"
                class="w-full rounded-lg border border-border bg-background px-3 py-2"
              />
            </div>

            <div>
              <label class="block text-sm mb-1">Destination port</label>
              <input
                v-model.number="form.destPort"
                type="number"
                class="w-full rounded-lg border border-border bg-background px-3 py-2"
              />
            </div>

            <div>
              <label class="block text-sm mb-1">TCP flags</label>
              <input
                v-model.number="form.flags"
                type="number"
                class="w-full rounded-lg border border-border bg-background px-3 py-2"
                placeholder="např. 0"
              />
            </div>

            <div>
              <label class="block text-sm mb-1">Limit count</label>
              <input
                v-model.number="form.limitCount"
                type="number"
                class="w-full rounded-lg border border-border bg-background px-3 py-2"
              />
            </div>

            <div>
              <label class="block text-sm mb-1">Min window size</label>
              <input
                v-model.number="form.minWin"
                type="number"
                class="w-full rounded-lg border border-border bg-background px-3 py-2"
              />
            </div>

            <div>
              <label class="block text-sm mb-1">Max window size</label>
              <input
                v-model.number="form.maxWin"
                type="number"
                class="w-full rounded-lg border border-border bg-background px-3 py-2"
              />
            </div>
          </div>

          <div class="mt-4 flex flex-wrap items-center gap-4">
            <label class="flex items-center gap-2 text-sm">
              <input v-model="form.save" type="checkbox" />
              <span>Uložit pravidlo</span>
            </label>
          </div>

          <div class="mt-4 flex items-center gap-3">
            <button
              @click="submitRule"
              :disabled="isSubmitting"
              class="rounded-lg px-4 py-2 text-sm font-medium border border-border bg-sidebar-accent hover:opacity-90 disabled:opacity-50"
            >
              {{ isSubmitting ? 'Odesílám...' : 'Poslat na server' }}
            </button>

            <button
              @click="fetchRules"
              :disabled="isLoading"
              class="rounded-lg px-4 py-2 text-sm font-medium border border-border hover:opacity-90 disabled:opacity-50"
            >
              Obnovit pravidla
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
                <th class="text-left px-4 py-3">Source port</th>
                <th class="text-left px-4 py-3">Destination port</th>
                <th class="text-left px-4 py-3">Flags</th>
                <th class="text-left px-4 py-3">Window</th>
                <th class="text-left px-4 py-3">Limit</th>
              </tr>
            </thead>

            <tbody>
              <tr
                v-for="rule in data"
                :key="rule.ID"
                class="border-b border-border last:border-b-0"
              >
                <td class="px-4 py-3">{{ rule.ID }}</td>
                <td class="px-4 py-3">{{ rule.ruleType ?? '-' }}</td>
                <td class="px-4 py-3">{{ formatAction(rule) }}</td>
                <td class="px-4 py-3">{{ rule.data?.sourcePort ?? '-' }}</td>
                <td class="px-4 py-3">{{ rule.data?.destPort ?? '-' }}</td>
                <td class="px-4 py-3">{{ formatFlags(rule) }}</td>
                <td class="px-4 py-3">{{ formatWindow(rule) }}</td>
                <td class="px-4 py-3">{{ rule.data?.limitCount ?? '-' }}</td>
              </tr>

              <tr v-if="data.length === 0">
                <td colspan="8" class="px-4 py-6 text-center text-muted-foreground">
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