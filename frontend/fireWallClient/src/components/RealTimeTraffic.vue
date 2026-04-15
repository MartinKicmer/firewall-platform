<script setup lang="ts">
import { Shield, LayoutDashboard, Search, ScrollText, Wifi, Trash2, Play, Square, Cpu } from 'lucide-vue-next'
import { ref, onUnmounted } from 'vue'
import api from '@/api/axios'
import TrafficTable from '@/components/TrafficTable.vue'
import type { TrafficRow } from '@/types/traffic'

const rows = ref<TrafficRow[]>([])
const listening = ref(false)
const isTransitioning = ref(false)
const selectedLayer = ref('L3')
let socket: WebSocket | null = null

const connectWebSocket = (): Promise<void> => {
  return new Promise((resolve) => {
    if (socket?.readyState === WebSocket.OPEN) return resolve()
    const host = import.meta.env.VITE_API_IP
    socket = new WebSocket(`ws://${host}:9090`)
    socket.onopen = () => { 
      listening.value = true
      resolve() 
    }
    socket.onmessage = (event) => {
      try {
        const parsed = JSON.parse(event.data)
        rows.value.unshift({
          timestamp: new Date().toLocaleTimeString(),
          layer: selectedLayer.value, 
          source: parsed.src || parsed.source_mac || '-',
          destination: parsed.dest || parsed.dest_mac || '-',
          sourcePort: parsed.sourcePort || '-',
          destinationPort: parsed.destPort || '-',
          protocol: parsed.protocol || '-',
          info: `TTL: ${parsed.ttl || '-'}, Len: ${parsed.total_len || parsed.len || '-'}`,
          action: parsed.action || '-',
          raw: parsed
        })
        if (rows.value.length > 500) rows.value.pop()
      } catch (e) { }
    }
    socket.onclose = () => { listening.value = false }
  })
}

const startCapture = async () => {
  isTransitioning.value = true
  rows.value = [] 
  try {
    await connectWebSocket()
    await api.get('/fireWall/redirect/start', { 
      params: { count: 0, layer: selectedLayer.value } 
    })
  } catch (e) {
    console.error(e)
  } finally {
    isTransitioning.value = false
  }
}

const stopCapture = async () => {
  isTransitioning.value = true
  try {
    if (socket) socket.close()
    await api.get('/fireWall/redirect/stop')
    listening.value = false
  } catch (e) {
    console.error(e)
  } finally {
    isTransitioning.value = false
  }
}

onUnmounted(() => {
  if (socket) socket.close()
})
</script>

<template>
  <div class="layout-wrapper">
    <aside class="sidebar">
      <div class="flex items-center gap-2.5 px-2 pb-6 pt-2 font-semibold text-lg text-white">
        <Shield :size="19" class="text-blue-500" /> 
        <span class="tracking-tight font-bold">PacketBlocker</span>
      </div>
      <nav class="flex flex-col gap-1.5">
        <router-link to="/" class="group nav-link">
          <LayoutDashboard :size="19" /> Dashboard
        </router-link>
        <router-link to="/rules" class="group nav-link">
          <Shield :size="19" /> Firewall Rules
        </router-link>
        <router-link to="/search" class="group nav-link">
          <Search :size="19" /> Search Rules</router-link>
        <router-link to="/logs" class="group nav-link">
          <ScrollText :size="19" class="text-blue-500" /> Real-time Traffic
        </router-link>
      </nav>
    </aside>

    <div class="main-container overflow-hidden">
      <header class="header-nav">
        <div class="flex items-center gap-3">
          <Wifi :size="20" :class="listening ? 'text-blue-500 animate-pulse' : 'text-pb-text-dim'" /> 
          <h1 class="text-xl font-extrabold tracking-tighter text-white font-sans">Real-time Traffic</h1>
        </div>
        <div :class="listening ? 'bg-blue-500/10 text-blue-400 border-blue-500/20' : 'bg-red-500/10 text-red-400 border-red-500/20'" class="badge-live">
            <span class="w-2 h-2 rounded-full" :class="listening ? 'bg-blue-500' : 'bg-red-500'"></span>
            {{ listening ? 'STREAMING' : 'STREAMING PAUSED' }}
        </div>
      </header>

      <main class="p-8 flex-1 flex flex-col min-h-0">
        <section class="stats-card flex-1 flex flex-col min-h-0 overflow-hidden !p-0">
          <div class="p-4 border-b border-pb-border bg-pb-card/50 flex justify-between items-center px-6">
            <div class="flex items-center gap-6">
                <div class="flex items-center gap-2 pr-4 border-r border-pb-border">
                    <h2 class="text-sm font-bold text-white uppercase tracking-tight">Live Traffic</h2>
                    <span class="text-[10px] bg-pb-border text-pb-text-dim px-2 py-0.5 rounded border border-pb-hover font-mono">
                      {{ rows.length }} pkts
                    </span>
                </div>

                <div class="flex items-center gap-4">
                  <button v-if="!listening" @click="startCapture" :disabled="isTransitioning" class="flex items-center gap-2 bg-green-600/10 text-green-500 hover:bg-green-600 hover:text-white px-4 py-1.5 rounded-lg text-[10px] font-bold border border-green-600/20 transition-all">
                    <Play :size="14" fill="currentColor" /> RUN
                  </button>

                  <button v-else @click="stopCapture" :disabled="isTransitioning" class="flex items-center gap-2 bg-red-600/10 text-red-400 hover:bg-red-600 hover:text-white px-4 py-1.5 rounded-lg text-[10px] font-bold border border-red-600/20 transition-all">
                    <Square :size="14" fill="currentColor" /> STOP
                  </button>
                </div>
            </div>

            <button @click="rows = []" class="group flex items-center gap-2 text-[10px] font-bold text-pb-text-dim hover:text-white bg-pb-border hover:bg-pb-hover px-3 py-1.5 rounded-lg border border-pb-hover transition-all">
              <Trash2 :size="14" class="group-hover:rotate-12 transition-transform" /> Clear memory
            </button>
          </div>

          <div class="flex-1 overflow-auto bg-pb-dark">
            <TrafficTable :rows="rows" />
            <div v-if="rows.length === 0" class="h-full flex flex-col items-center justify-center text-pb-text-dim space-y-4 opacity-50">
               <Wifi :size="48" stroke-width="1" />
               <p class="text-sm italic font-sans">Vyberte vrstvu a spusťte sledování provozu.</p>
            </div>
          </div>
        </section>
      </main>
    </div>
  </div>
</template>