<script setup lang="ts">
import { Shield, LayoutDashboard, ScrollText } from 'lucide-vue-next'
import { ref, onMounted } from 'vue'

type RuleData = {
  permit?: boolean
  source?: [string, number]
  dest?: [string, number]
  destPort?: number
  sourcePort?: number
  protocol?: number
  minTTL?: number
  maxTTL?: number
  TOS?: number
  allowFrag?: boolean
  limitCount?: number
}

type FirewallRule = {
  ID: number
  ruleType: string
  save?: boolean
  update?: boolean
  data?: RuleData
}

const data = ref<FirewallRule[]>([])
const isLoading = ref(false)
const error = ref<string | null>(null)

const isSubmitting = ref(false)
const submitMessage = ref<string | null>(null)
const submitError = ref<string | null>(null)

const form = ref({
  ID: 10,
  permit: false,
  sourceIp: '192.168.88.77',
  sourceMask: 24,
  destIp: 'none',
  destMask: -1,
  protocol: -1,
  minTTL: -1,
  maxTTL: -1,
  TOS: -1,
  allowFrag: true,
  save: false,
  update: false,
})

const fetchRules = async () => {
  isLoading.value = true
  error.value = null

  try {
    const response = await fetch(
      'http://192.168.50.1:8080/fireWall/selectRule/0?ID=1&layer=L4Simple&fromMemory=1',
      {
        method: 'GET',
        headers: {
          Accept: 'application/json',
          'Content-Type': 'application/json',
        },
      }
    )

    if (!response.ok) {
      error.value = `Chyba serveru: ${response.status}`
      return
    }

    const result = await response.json()
    data.value = result
    console.log('Firewall rules:', result)
  } catch (e) {
    error.value = 'Nelze se spojit s bránou. Zkontrolujte připojení k síti.'
    console.error('Network error:', e)
  } finally {
    isLoading.value = false
  }
}

const submitRule = async () => {
  isSubmitting.value = true
  submitMessage.value = null
  submitError.value = null

  const payload = buildL3Payload()

  try {
    const response = await fetch('http://192.168.50.1:8080/fireWall/addRule', {
      method: 'POST',
      headers: {
        Accept: 'application/json',
        'Content-Type': 'application/json',
      },
      body: JSON.stringify(payload),
    })

    const text = await response.text()
    console.log('POST payload:', payload)
    console.log('POST response:', text)

    if (!response.ok) {
      submitError.value = `Chyba při odeslání: ${response.status} ${text}`
      return
    }

    submitMessage.value = 'Pravidlo bylo úspěšně odesláno na server.'
    await fetchRules()
  } catch (e) {
    submitError.value = 'Nepodařilo se odeslat pravidlo na server.'
    console.error('Submit error:', e)
  } finally {
    isSubmitting.value = false
  }
}

const formatAction = (rule: FirewallRule) => {
  return rule.data?.permit === true ? 'ALLOW' : 'DENY'
}

const formatAddress = (address?: [string, number]) => {
  if (Array.isArray(address) && address.length >= 2) {
    return `${address[0]}/${address[1]}`
  }
  return '-'
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
          <h2 class="text-base font-semibold mb-4">Vytvořit L3 pravidlo</h2>

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
              <label class="block text-sm mb-1">Source IP</label>
              <input
                v-model="form.sourceIp"
                type="text"
                class="w-full rounded-lg border border-border bg-background px-3 py-2"
                placeholder="192.168.88.77"
              />
            </div>

            <div>
              <label class="block text-sm mb-1">Source maska</label>
              <input
                v-model.number="form.sourceMask"
                type="number"
                class="w-full rounded-lg border border-border bg-background px-3 py-2"
              />
            </div>

            <div>
              <label class="block text-sm mb-1">Destination IP</label>
              <input
                v-model="form.destIp"
                type="text"
                class="w-full rounded-lg border border-border bg-background px-3 py-2"
                placeholder="none"
              />
            </div>

            <div>
              <label class="block text-sm mb-1">Destination maska</label>
              <input
                v-model.number="form.destMask"
                type="number"
                class="w-full rounded-lg border border-border bg-background px-3 py-2"
              />
            </div>

            <div>
              <label class="block text-sm mb-1">Protocol</label>
              <input
                v-model.number="form.protocol"
                type="number"
                class="w-full rounded-lg border border-border bg-background px-3 py-2"
              />
            </div>

            <div>
              <label class="block text-sm mb-1">TOS</label>
              <input
                v-model.number="form.TOS"
                type="number"
                class="w-full rounded-lg border border-border bg-background px-3 py-2"
              />
            </div>

            <div>
              <label class="block text-sm mb-1">Min TTL</label>
              <input
                v-model.number="form.minTTL"
                type="number"
                class="w-full rounded-lg border border-border bg-background px-3 py-2"
              />
            </div>

            <div>
              <label class="block text-sm mb-1">Max TTL</label>
              <input
                v-model.number="form.maxTTL"
                type="number"
                class="w-full rounded-lg border border-border bg-background px-3 py-2"
              />
            </div>
          </div>

          <div class="mt-4 flex items-center gap-3">
            <button
              @click="submitRule"
              :disabled="isSubmitting"
              class="rounded-lg px-4 py-2 text-sm font-medium border border-border bg-sidebar-accent hover:opacity-90 disabled:opacity-50"
            >
              {{ isSubmitting ? 'Odesílám...' : 'Poslat na server' }}
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
                <th class="text-left px-4 py-3">Zdroj</th>
                <th class="text-left px-4 py-3">Cíl</th>
                <th class="text-left px-4 py-3">Port</th>
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
                <td class="px-4 py-3">{{ formatAddress(rule.data?.source) }}</td>
                <td class="px-4 py-3">{{ formatAddress(rule.data?.dest) }}</td>
                <td class="px-4 py-3">{{ rule.data?.destPort ?? rule.data?.sourcePort ?? '-' }}</td>
              </tr>

              <tr v-if="data.length === 0">
                <td colspan="6" class="px-4 py-6 text-center text-muted-foreground">
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