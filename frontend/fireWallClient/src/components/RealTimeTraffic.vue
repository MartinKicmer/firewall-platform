<script setup lang="ts">
import { Shield, LayoutDashboard, ScrollText } from 'lucide-vue-next'
import { ref, onUnmounted } from 'vue'
import api from '@/api/axios'
import TrafficControlPanel from '@/components/TrafficControlPanel.vue'
import TrafficTable from '@/components/TrafficTable.vue'
import type { TrafficControlSubmit, TrafficRow } from '@/types/traffic'

const isStarting = ref(false)
const startMessage = ref<string | null>(null)
const startError = ref<string | null>(null)
const rows = ref<TrafficRow[]>([])
const listening = ref(false)

let socket: WebSocket | null = null
const MAX_ROWS = 100

const protocolToText = (protocol?: number | string) => {
  const value = typeof protocol === 'string' ? Number(protocol) : protocol

  if (value === 6) return 'TCP'
  if (value === 17) return 'UDP'
  if (value === 1) return 'ICMP'
  return String(protocol ?? '-')
}

const pushRow = (row: TrafficRow) => {
  rows.value.unshift(row)

  if (rows.value.length > MAX_ROWS) {
    rows.value = rows.value.slice(0, MAX_ROWS)
  }
}

const clearRows = () => {
  rows.value = []
}

const buildInfo = (raw: Record<string, unknown>) => {
  const parts: string[] = []

  if ('type' in raw) parts.push(`Type: ${String(raw.type)}`)
  if ('ttl' in raw) parts.push(`TTL: ${String(raw.ttl)}`)
  if ('total_len' in raw) parts.push(`Len: ${String(raw.total_len)}`)
  if ('tos' in raw) parts.push(`TOS: ${String(raw.tos)}`)
  if ('id' in raw) parts.push(`ID: ${String(raw.id)}`)
  if ('checksum' in raw) parts.push(`Checksum: ${String(raw.checksum)}`)

  return parts.length > 0 ? parts.join(', ') : '-'
}

const normalizeTrafficRow = (
  payload: Record<string, unknown>,
  layer: string,
  action: 0 | 1
): TrafficRow => {
  return {
    timestamp: new Date().toLocaleTimeString(),
    layer,
    action: action === 1 ? 'ALLOW' : 'DENY',
    source:
      (payload.src as string | undefined) ??
      (payload.source as string | undefined) ??
      (payload.sourceMAC as string | undefined) ??
      '-',
    destination:
      (payload.dest as string | undefined) ??
      (payload.destination as string | undefined) ??
      (payload.destMAC as string | undefined) ??
      '-',
    sourcePort: (payload.sourcePort as number | string | undefined) ?? '-',
    destinationPort: (payload.destPort as number | string | undefined) ?? '-',
    protocol: protocolToText(payload.protocol as number | string | undefined),
    info: buildInfo(payload),
    raw: payload,
  }
}
let manualClose = false

const connectWebSocket = (): Promise<void> => {
  return new Promise((resolve, reject) => {
    if (socket && socket.readyState === WebSocket.OPEN) {
      listening.value = true
      resolve()
      return
    }

    if (socket && socket.readyState === WebSocket.CONNECTING) {
      resolve()
      return
    }

    manualClose = false

    socket = new WebSocket('ws://192.168.50.1:9090')

    socket.onopen = () => {
      console.log('WebSocket připojen')
      listening.value = true
      startError.value = null
      resolve()
    }

    socket.onmessage = (event) => {
      console.log('WS MESSAGE:', event.data)

      if (typeof event.data !== 'string') return

      if (event.data === 'Full packet stream from firewall') {
        pushRow({
          timestamp: new Date().toLocaleTimeString(),
          layer: 'SYSTEM',
          action: undefined,
          source: '-',
          destination: '-',
          sourcePort: '-',
          destinationPort: '-',
          protocol: '-',
          info: event.data,
          raw: { message: event.data },
        })
        return
      }

      try {
        const parsed = JSON.parse(event.data) as Record<string, unknown>

        const detectedLayer =
          parsed.type === 'IPv4Datagram'
            ? 'L3'
            : typeof parsed.type === 'string'
              ? parsed.type
              : '-'

        pushRow({
          timestamp: new Date().toLocaleTimeString(),
          layer: detectedLayer,
          action: undefined,
          source: (parsed.src as string | undefined) ?? '-',
          destination: (parsed.dest as string | undefined) ?? '-',
          sourcePort: (parsed.sourcePort as number | string | undefined) ?? '-',
          destinationPort: (parsed.destPort as number | string | undefined) ?? '-',
          protocol: protocolToText(parsed.protocol as number | string | undefined),
          info: buildInfo(parsed),
          raw: parsed,
        })
      } catch {
        pushRow({
          timestamp: new Date().toLocaleTimeString(),
          layer: '-',
          action: undefined,
          source: '-',
          destination: '-',
          sourcePort: '-',
          destinationPort: '-',
          protocol: '-',
          info: String(event.data),
          raw: { message: String(event.data) },
        })
      }
    }

    socket.onerror = (err) => {
      console.error('WebSocket error:', err)
    }

    socket.onclose = (event) => {
      console.log('WebSocket odpojen', {
        code: event.code,
        reason: event.reason,
        wasClean: event.wasClean,
      })

      listening.value = false
      socket = null

      if (!event.wasClean) {
        startError.value = `WebSocket spojení bylo neočekávaně ukončeno (code ${event.code}).`
      }
    }

    setTimeout(() => {
      if (!socket || socket.readyState !== WebSocket.OPEN) {
        reject(new Error('WebSocket spojení se nepodařilo navázat'))
      }
    }, 3000)
  })
}

onUnmounted(() => {
  manualClose = true
  if (socket) {
    socket.close()
    socket = null
  }
})

const startRedirect = async ({ action, layer, count }: TrafficControlSubmit) => {
  isStarting.value = true
  startMessage.value = null
  startError.value = null

  const safeCount =
    typeof count === 'number' && Number.isFinite(count) && count > 0 ? count : 10

  try {
    await connectWebSocket()

    await api.get(`/fireWall/redirect/${action}`, {
      params: {
        count: safeCount,
        layer,
      },
    })

    listening.value = true
    startMessage.value = `Sledování pro ${layer} bylo spuštěno.`

    pushRow(
      normalizeTrafficRow(
        { info: `Redirect spuštěn pro ${layer}, count=${String(safeCount)}` },
        layer,
        action
      )
    )
  } catch (e: any) {
    startError.value =
      e.response?.status != null
        ? `Chyba serveru: ${e.response.status}`
        : e.message ?? 'Nepodařilo se spustit sledování provozu.'
  } finally {
    isStarting.value = false
  }
}

onUnmounted(() => {
  if (socket) {
    socket.close()
    socket = null
  }
})
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
        <h1 class="text-lg font-semibold">Real-time Trafic</h1>

        <div
          class="inline-flex items-center gap-2 px-3 py-2 rounded-full text-xs font-semibold border"
          :class="
            listening
              ? 'bg-green-500/10 text-green-600 border-green-500/20'
              : 'bg-muted text-muted-foreground border-border'
          "
        >
          <span
            class="w-2 h-2 rounded-full"
            :class="listening ? 'bg-green-500' : 'bg-muted-foreground'"
          ></span>
          {{ listening ? 'Listening' : 'Stopped' }}
        </div>
      </header>

      <main class="p-6 space-y-6">
        <section class="rounded-xl border border-border bg-card p-6">
          <h2 class="text-base font-semibold mb-4">Nastavení sledování</h2>

          <TrafficControlPanel
            :loading="isStarting"
            @start="startRedirect"
          />

          <div class="mt-4 flex items-center gap-3">
            <span v-if="startMessage" class="text-sm text-green-600">
              {{ startMessage }}
            </span>

            <span v-if="startError" class="text-sm text-red-500">
              {{ startError }}
            </span>
          </div>
        </section>

        <section class="rounded-xl border border-border bg-card p-6">
          <div class="mb-4 flex items-center justify-between">
            <h2 class="text-base font-semibold">Live traffic</h2>

            <button
              @click="clearRows"
              class="rounded-lg px-3 py-2 text-sm font-medium border border-border hover:opacity-90"
            >
              Vyčistit tabulku
            </button>
          </div>

          <TrafficTable :rows="rows" />
        </section>
      </main>
    </div>
  </div>
</template>