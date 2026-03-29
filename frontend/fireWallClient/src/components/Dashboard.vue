<script setup lang="ts">
import { Shield, LayoutDashboard, ScrollText } from 'lucide-vue-next'
import { ref, computed, onMounted } from 'vue'
import api from '@/api/axios'

type Packet = {
  src?: string
  dest?: string
  protocol?: number | string
  sourcePort?: number
  destPort?: number
  type?: string
  ttl?: number
  total_len?: number
}

const packets = ref<Packet[]>([])
const isLoading = ref(false)
const error = ref<string | null>(null)

const protocolToText = (protocol?: number | string) => {
  const value = typeof protocol === 'string' ? Number(protocol) : protocol

  if (value === 6) return 'TCP'
  if (value === 17) return 'UDP'
  if (value === 1) return 'ICMP'
  return String(protocol ?? '-')
}

const countOccurrences = (items: (string | number | undefined)[]) => {
  const map = new Map<string, number>()

  for (const item of items) {
    if (item === undefined || item === null || item === '') continue
    const key = String(item)
    map.set(key, (map.get(key) ?? 0) + 1)
  }

  return map
}

const getTopEntry = (map: Map<string, number>) => {
  let topKey = '-'
  let topValue = 0

  for (const [key, value] of map.entries()) {
    if (value > topValue) {
      topKey = key
      topValue = value
    }
  }

  return { key: topKey, count: topValue }
}

const totalPackets = computed(() => packets.value.length)

const topSourceIp = computed(() => {
  const counts = countOccurrences(packets.value.map((p) => p.src))
  return getTopEntry(counts)
})

const topDestinationIp = computed(() => {
  const counts = countOccurrences(packets.value.map((p) => p.dest))
  return getTopEntry(counts)
})

const topProtocol = computed(() => {
  const counts = countOccurrences(
    packets.value.map((p) => protocolToText(p.protocol))
  )
  return getTopEntry(counts)
})

const topPort = computed(() => {
  const counts = countOccurrences([
    ...packets.value.map((p) => p.sourcePort),
    ...packets.value.map((p) => p.destPort),
  ])
  return getTopEntry(counts)
})

const avgPacketLength = computed(() => {
  const lengths = packets.value
    .map((p) => p.total_len)
    .filter((value): value is number => typeof value === 'number')

  if (lengths.length === 0) return '-'

  const sum = lengths.reduce((acc, value) => acc + value, 0)
  return Math.round(sum / lengths.length)
})

const recentPackets = computed(() => packets.value.slice(0, 10))

const fetchStatsData = async () => {
  isLoading.value = true
  error.value = null

  try {
    const response = await api.get('/fireWall/redirect/1', {
      params: {
        count: 20,
        layer: 'L3',
      },
    })

    packets.value = Array.isArray(response.data) ? response.data : []
  } catch (e: any) {
    error.value =
      e.response?.status != null
        ? `Chyba serveru: ${e.response.status}`
        : 'Nepodařilo se načíst statistiky.'
    packets.value = []
  } finally {
    isLoading.value = false
  }
}

onMounted(fetchStatsData)
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
        <h1 class="text-lg font-semibold">Dashboard</h1>

        <div
          class="inline-flex items-center gap-2 px-3 py-2 rounded-full text-xs font-semibold
                 bg-green-500/10 text-green-600 border border-green-500/20"
        >
          <span class="w-2 h-2 rounded-full bg-green-500"></span>
          System Active
        </div>
      </header>

      <main class="p-6 space-y-6">
        <div v-if="isLoading" class="text-sm text-muted-foreground">
          Načítám statistiky...
        </div>

        <div v-else-if="error" class="text-sm text-red-500">
          {{ error }}
        </div>

        <template v-else>
          <section class="grid grid-cols-1 md:grid-cols-2 xl:grid-cols-5 gap-4">
            <div class="rounded-xl border border-border bg-card p-5">
              <p class="text-sm text-muted-foreground">Celkem packetů</p>
              <p class="text-2xl font-semibold mt-2">{{ totalPackets }}</p>
            </div>

            <div class="rounded-xl border border-border bg-card p-5">
              <p class="text-sm text-muted-foreground">Nejčastější zdrojová IP</p>
              <p class="text-lg font-semibold mt-2 break-all">{{ topSourceIp.key }}</p>
              <p class="text-xs text-muted-foreground mt-1">{{ topSourceIp.count }}×</p>
            </div>

            <div class="rounded-xl border border-border bg-card p-5">
              <p class="text-sm text-muted-foreground">Nejčastější cílová IP</p>
              <p class="text-lg font-semibold mt-2 break-all">{{ topDestinationIp.key }}</p>
              <p class="text-xs text-muted-foreground mt-1">{{ topDestinationIp.count }}×</p>
            </div>

            <div class="rounded-xl border border-border bg-card p-5">
              <p class="text-sm text-muted-foreground">Nejčastější protokol</p>
              <p class="text-2xl font-semibold mt-2">{{ topProtocol.key }}</p>
              <p class="text-xs text-muted-foreground mt-1">{{ topProtocol.count }}×</p>
            </div>

            <div class="rounded-xl border border-border bg-card p-5">
              <p class="text-sm text-muted-foreground">Nejčastější port</p>
              <p class="text-2xl font-semibold mt-2">{{ topPort.key }}</p>
              <p class="text-xs text-muted-foreground mt-1">{{ topPort.count }}×</p>
            </div>
          </section>

          <section class="grid grid-cols-1 xl:grid-cols-2 gap-4">
            <div class="rounded-xl border border-border bg-card p-5">
              <p class="text-sm text-muted-foreground">Průměrná délka packetu</p>
              <p class="text-2xl font-semibold mt-2">
                {{ avgPacketLength === '-' ? '-' : `${avgPacketLength} B` }}
              </p>
            </div>

            <div class="rounded-xl border border-border bg-card p-5">
              <p class="text-sm text-muted-foreground">Zachycený typ provozu</p>
              <p class="text-lg font-semibold mt-2">IPv4 / redirect statistiky</p>
              <p class="text-xs text-muted-foreground mt-1">
                Data jsou počítaná z posledního načtení packetů.
              </p>
            </div>
          </section>

          <section class="rounded-xl border border-border bg-card p-6">
            <div class="flex items-center justify-between mb-4">
              <h2 class="text-base font-semibold">Poslední zachycené packety</h2>

              <button
                @click="fetchStatsData"
                class="rounded-lg px-4 py-2 text-sm font-medium border border-border bg-sidebar-accent hover:opacity-90"
              >
                Obnovit
              </button>
            </div>

            <div class="overflow-x-auto">
              <table class="w-full text-sm">
                <thead class="bg-muted/40 border-b border-border">
                  <tr>
                    <th class="text-left px-4 py-3">Typ</th>
                    <th class="text-left px-4 py-3">Zdroj</th>
                    <th class="text-left px-4 py-3">Cíl</th>
                    <th class="text-left px-4 py-3">Protokol</th>
                    <th class="text-left px-4 py-3">Src Port</th>
                    <th class="text-left px-4 py-3">Dst Port</th>
                    <th class="text-left px-4 py-3">TTL</th>
                    <th class="text-left px-4 py-3">Délka</th>
                  </tr>
                </thead>

                <tbody>
                  <tr
                    v-for="(packet, index) in recentPackets"
                    :key="index"
                    class="border-b border-border last:border-b-0"
                  >
                    <td class="px-4 py-3">{{ packet.type ?? '-' }}</td>
                    <td class="px-4 py-3">{{ packet.src ?? '-' }}</td>
                    <td class="px-4 py-3">{{ packet.dest ?? '-' }}</td>
                    <td class="px-4 py-3">{{ protocolToText(packet.protocol) }}</td>
                    <td class="px-4 py-3">{{ packet.sourcePort ?? '-' }}</td>
                    <td class="px-4 py-3">{{ packet.destPort ?? '-' }}</td>
                    <td class="px-4 py-3">{{ packet.ttl ?? '-' }}</td>
                    <td class="px-4 py-3">{{ packet.total_len ?? '-' }}</td>
                  </tr>

                  <tr v-if="recentPackets.length === 0">
                    <td colspan="8" class="px-4 py-6 text-center text-muted-foreground">
                      Nejsou dostupná žádná data.
                    </td>
                  </tr>
                </tbody>
              </table>
            </div>
          </section>
        </template>
      </main>
    </div>
  </div>
</template>