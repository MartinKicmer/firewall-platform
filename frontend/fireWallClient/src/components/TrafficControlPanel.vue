<script setup lang="ts">
import { ref, watch } from 'vue'
import type { TrafficControlSubmit, TrafficLayer } from '@/types/traffic'

const emit = defineEmits<{
  start: [event: TrafficControlSubmit]
}>()

const props = defineProps<{
  loading?: boolean
}>()

const form = ref({
  permit: false,
  layer: 'L4TCP' as TrafficLayer,
  countMode: 'number' as 'number' | 'all',
  count: 10,
})

const submit = () => {
  emit('start', {
    action: form.value.permit ? 1 : 0,
    layer: form.value.layer,
    count: form.value.countMode === 'all' ? 'all' : form.value.count,
  })
}

watch(
  () => form.value.countMode,
  (mode) => {
    if (mode === 'all') {
      form.value.count = 10
    }
  }
)
</script>

<template>
  <div>
    <div class="grid grid-cols-1 md:grid-cols-2 xl:grid-cols-4 gap-4">
      <div>
        <label class="block text-sm mb-1">Vrstva</label>
        <select
          v-model="form.layer"
          class="w-full rounded-lg border border-border bg-background px-3 py-2"
        >
          <option value="L2">L2</option>
          <option value="L3">L3</option>
          <option value="L4Simple">L4Simple</option>
          <option value="L4TCP">L4TCP</option>
        </select>
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
        <label class="block text-sm mb-1">Režim počtu</label>
        <select
          v-model="form.countMode"
          class="w-full rounded-lg border border-border bg-background px-3 py-2"
        >
          <option value="number">Konkrétní počet</option>
          <option value="all">Vše</option>
        </select>
      </div>

      <div>
        <label class="block text-sm mb-1">Count</label>
        <input
          v-model.number="form.count"
          :disabled="form.countMode === 'all'"
          type="number"
          min="1"
          class="w-full rounded-lg border border-border bg-background px-3 py-2 disabled:opacity-50"
        />
      </div>
    </div>

    <div class="mt-4">
      <button
        @click="submit"
        :disabled="props.loading"
        class="rounded-lg px-4 py-2 text-sm font-medium border border-border bg-sidebar-accent hover:opacity-90 disabled:opacity-50"
      >
        {{ props.loading ? 'Spouštím...' : 'Spustit sledování' }}
      </button>
    </div>
  </div>
</template>