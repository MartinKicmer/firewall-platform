import { createRouter, createWebHistory } from 'vue-router'
import Dashboard from '@/components/Dashboard.vue'
import FirewallRules from '@/components/FirewallRules.vue'

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
]

const router = createRouter({
  history: createWebHistory(),
  routes,
})

export default router