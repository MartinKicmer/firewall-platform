import { createApp } from 'vue'
import App from './App.vue'
import router from './router'
import './style.css'
const app = createApp(App)

const API = '${window.location.protocol}//{window.location.hostname}:8080'

app.use(router)

app.mount('#app')
