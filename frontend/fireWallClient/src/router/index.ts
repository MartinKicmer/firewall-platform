import { createRouter, createWebHistory } from 'vue-router'
import Dashboard from '@/components/Dashboard.vue'
import FirewallRules from '@/components/FirewallRules.vue'
import RealTimeTraffic from '@/components/RealTimeTraffic.vue'
import SearchRules from '@/components/SearchRules.vue'

const routes = [
  {
    path: '/',
    name: 'Dashboard',
    component: Dashboard,
  },
  {
    path: '/rules',
    name: 'FirewallRules',
    component: FirewallRules,
  },
  {
    path: '/logs',
    name: 'RealTimeTraffic',
    component: RealTimeTraffic,
  },

  {
    path: '/search',
    name: 'SearchRules',
    component: SearchRules,
  },
]

const router = createRouter({
  history: createWebHistory(),
  routes,
})

export default router