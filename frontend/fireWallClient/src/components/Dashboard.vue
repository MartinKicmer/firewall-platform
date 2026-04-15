<script setup lang="ts">
import { Shield, LayoutDashboard, ScrollText, Globe, Activity, PieChart, Search } from 'lucide-vue-next'
import { ref, computed, onMounted, onUnmounted } from 'vue'

const packets = ref<any[]>([])
let socket: WebSocket | null = null

const connectLiveStats = () => {
  const host = import.meta.env.VITE_API_IP
  socket = new WebSocket(`ws://${host || 'localhost'}:9090`)
  
  socket.onmessage = (event) => {
    try {
      const parsed = JSON.parse(event.data)
      packets.value.unshift(parsed)
    } catch (e) { console.error("WS Error:", e) }
  }
}

const getStrokeDash = (percentage: number) => {
  const radius = 15.9155;
  const circumference = 2 * Math.PI * radius;
  return `${(percentage * circumference) / 100} ${circumference}`;
}

const chartColors = ['#3b82f6', '#10b981', '#f59e0b', '#ef4444', '#8b5cf6'];

const protocolToText = (p?: any) => {
  const v = Number(p);
  if (v === 6) return 'TCP'; if (v === 17) return 'UDP'; if (v === 1) return 'ICMP';
  return String(p ?? '-');
}

const getTopEntries = (items: any[], limit = 5) => {
    const map = new Map<string, number>();
    items.forEach(i => { if(i) map.set(String(i), (map.get(String(i)) ?? 0) + 1) });
    const total = Array.from(map.values()).reduce((a, b) => a + b, 0);
    return Array.from(map.entries())
        .sort((a, b) => b[1] - a[1])
        .slice(0, limit)
        .map(([key, count]) => ({ key, count, percentage: total > 0 ? Math.round((count / total) * 100) : 0 }));
}

const totalPackets = computed(() => packets.value.length)
const topSources = computed(() => getTopEntries(packets.value.map(p => p.src || p.source)))
const topDestinations = computed(() => getTopEntries(packets.value.map(p => p.dest || p.destination)))
const topProtocols = computed(() => getTopEntries(packets.value.map(p => protocolToText(p.protocol))))

onMounted(() => connectLiveStats())
onUnmounted(() => socket?.close())
</script>

<template>
  <div class="layout-wrapper">
    <aside class="sidebar">
      <div class="flex items-center gap-2.5 px-2 pb-6 pt-2 font-semibold text-lg text-white">
        <Shield :size="22" class="text-blue-500" /> <span class="tracking-tight font-bold">PacketBlocker</span>
      </div>
      <nav class="flex flex-col gap-1.5 flex-grow">
        <router-link to="/" class="group nav-link nav-link-active"><LayoutDashboard :size="19" class="text-blue-500" /> Dashboard</router-link>
        <router-link to="/rules" class="group nav-link"><Shield :size="19" /> Firewall Rules</router-link>
        <router-link to="/search" class="group nav-link"><Search :size="19"/> Search Rules</router-link>
        <router-link to="/logs" class="group nav-link"><ScrollText :size="19" /> Real-time Traffic</router-link>
      </nav>
    </aside>

    <div class="main-container">
      <header class="header-nav">
        <div class="flex items-center gap-3 font-black tracking-tighter text-white">
          <Activity class="text-green-500 animate-pulse" /> Live Analysis
        </div>
        <div class="badge-live bg-green-500/10 text-green-400 border-green-500/20">
            <span class="w-2 h-2 rounded-full bg-green-500"></span> SYSTEM ACTIVE
        </div>
      </header>

      <main class="p-8 space-y-8">
        <section class="grid grid-cols-1 md:grid-cols-3 gap-6">
          <div class="stats-card stats-card-accent">
            <div class="flex items-center justify-between mb-4">
                <p class="text-xs text-pb-text-muted uppercase font-bold tracking-wider">Total captured</p>
                <LayoutDashboard :size="16" class="text-blue-500" />
            </div>
            <p class="text-4xl font-black tracking-tighter text-white">{{ totalPackets.toLocaleString('cs-CZ') }}</p>
            <p class="text-[10px] text-green-400 mt-2 font-bold uppercase tracking-widest flex items-center gap-1"><Activity :size="12" /> Real time</p>
          </div>
          <div v-for="(item, i) in [{t:'Top Source IP', d:topSources}, {t:'Top Destination', d:topDestinations}]" :key="i" class="stats-card">
            <h3 class="text-[10px] text-pb-text-muted uppercase font-bold tracking-widest mb-4">{{ item.t }}</h3>
            <div class="flex flex-col gap-1">
                <span class="text-xs font-mono text-white bg-pb-dark px-2 py-1 rounded border border-pb-border truncate">{{ item.d[0]?.key || '-' }}</span>
                <span class="text-[10px] text-blue-500 font-bold mt-1 uppercase">{{ item.d[0]?.count || 0 }}x hits</span>
            </div>
          </div>
        </section>

        <section class="stats-card p-8">
            <div class="flex items-center justify-between mb-10">
                <h2 class="text-lg font-bold tracking-tight text-white flex items-center gap-2.5 uppercase"><PieChart :size="20" class="text-blue-500" /> Network Distribution</h2>
            </div>
            <div class="grid grid-cols-1 lg:grid-cols-2 gap-16">
                <div v-for="(chart, idx) in [{title: 'Protocols', data: topProtocols}, {title: 'Source IP', data: topSources}]" :key="idx" class="flex flex-col items-center">
                    <p class="text-[10px] font-bold uppercase tracking-widest text-pb-text-muted mb-8">{{ chart.title }}</p>
                    <div class="relative w-48 h-48 mb-8">
                        <svg viewBox="0 0 42 42" class="w-full h-full transform -rotate-90">
                            <circle cx="21" cy="21" r="15.9155" fill="transparent" stroke="#27272A" stroke-width="4"></circle>
                            <circle v-for="(entry, i) in chart.data" :key="i" cx="21" cy="21" r="15.9155" fill="transparent" :stroke="chartColors[i]" stroke-width="4" :stroke-dasharray="getStrokeDash(entry.percentage)" class="transition-all duration-700"></circle>
                        </svg>
                        <div v-if="idx === 0" class="absolute inset-0 flex flex-col items-center justify-center">
                            <span class="text-2xl font-black tracking-tighter">{{ totalPackets }}</span>
                            <span class="text-[8px] uppercase text-pb-text-dim font-bold tracking-tighter">Packets</span>
                        </div>
                    </div>
                    <div class="w-full max-w-sm space-y-3">
                        <div v-for="(entry, i) in chart.data" :key="i" class="flex items-center justify-between text-[11px]">
                            <div class="flex items-center gap-3">
                                <div class="w-2 h-2 rounded-full" :style="{ backgroundColor: chartColors[i] }"></div>
                                <span class="font-bold text-white uppercase tracking-wider truncate max-w-[150px]">{{ entry.key }}</span>
                            </div>
                            <span class="text-pb-text-muted font-mono">{{ idx === 0 ? entry.percentage + '%' : entry.count + 'x' }}</span>
                        </div>
                    </div>
                </div>
            </div>
        </section>
      </main>
    </div>
  </div>
</template>