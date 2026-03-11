<script setup>
import { Shield, LayoutDashboard, ScrollText } from 'lucide-vue-next'
import { ref, onMounted } from 'vue'

const data = ref(null)
const isLoading = ref(false)
const error = ref(null)

const fetchData = async () => {
    isLoading.value = true
    error.value = null
    
    try {
        const response = await fetch('http://172.20.10.12:8081/fireWall/selectRule/0?ID=12&layer=L4Simple&fromMemory=1', {
            method: 'GET',
            headers: {
                'Accept': 'application/json',
                'Content-Type': 'application/json'
            }
        })
        
        if (response.ok) {
            data.value = await response.json()
            console.log(data.value);
        } else {
            error.value = `Chyba serveru: ${response.status}`
        }
    } catch (e) {
        error.value = "Nelze se spojit s bránou. Zkontrolujte připojení k síti."
        console.error("Network error:", e)
    } finally {
        isLoading.value = false
    }
}
onMounted(fetchData);


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
      <header class="h-14 bg-card border-b border-border flex items-center justify-end px-4">
        <div class="inline-flex items-center gap-2 px-3 py-2 rounded-full text-xs font-semibold
                    bg-green-500/10 text-green-600 border border-green-500/20">
          <span class="w-2 h-2 rounded-full bg-green-500"></span>
          System Active
        </div>
      </header>

      <main class="p-6">
          <ul>
            <li v-for="rule in data" :key="rule.id">
              ID: {{ rule.ID }} | Protokol: {{ rule.ruleType }} | Akce: {{ rule.action }}
            </li>
          </ul>
      </main>
    </div>
  </div>
</template>