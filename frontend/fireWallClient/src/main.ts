import { createApp } from 'vue'
import App from './App.vue'
import router from './router'
import './style.css'
const app = createApp(App)

const API = '192.168.50.1:8080'

app.use(router)

app.mount('#app')
