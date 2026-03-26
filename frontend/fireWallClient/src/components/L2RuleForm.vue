<script setup lang="ts">
import { ref } from 'vue'

type SubmitEvent = {
  action: number
  payload: {
    ID: number
    layer: 'L2'
    save: boolean
    sourceMAC: string
    destMAC: string
    limitCount: number
  }
}

const emit = defineEmits<{
  submit: [event: SubmitEvent]
}>()

const form = ref({
  ID: 1,
  permit: false,
  sourceMAC: '',
  destMAC: '',
  limitCount: 1,
  save: false,
})

const onSubmit = () => {
  emit('submit', {
    action: form.value.permit ? 1 : 0,
    payload: {
      ID: form.value.ID,
      layer: 'L2',
      save: form.value.save,
      sourceMAC: form.value.sourceMAC,
      destMAC: form.value.destMAC,
      limitCount: form.value.limitCount,
    },
  })
}
</script>

<template>
  <div>
    <div class="grid grid-cols-1 md:grid-cols-2 gap-4">
      <div>
        <label class="block text-sm mb-1">ID</label>
        <input
          v-model.number="form.ID"
          type="number"
          class="w-full rounded-lg border border-border bg-background px-3 py-2"
        />
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
        <label class="block text-sm mb-1">Source MAC</label>
        <input
          v-model="form.sourceMAC"
          type="text"
          placeholder="AA:BB:CC:DD:EE:FF"
          class="w-full rounded-lg border border-border bg-background px-3 py-2"
        />
      </div>

      <div>
        <label class="block text-sm mb-1">Destination MAC</label>
        <input
          v-model="form.destMAC"
          type="text"
          placeholder="AA:BB:CC:DD:EE:FF"
          class="w-full rounded-lg border border-border bg-background px-3 py-2"
        />
      </div>

      <div>
        <label class="block text-sm mb-1">Limit count</label>
        <input
          v-model.number="form.limitCount"
          type="number"
          class="w-full rounded-lg border border-border bg-background px-3 py-2"
        />
      </div>
    </div>

    <div class="mt-4 flex items-center gap-4">
      <label class="flex items-center gap-2 text-sm">
        <input v-model="form.save" type="checkbox" />
        <span>Uložit pravidlo</span>
      </label>
    </div>

    <div class="mt-4">
      <button
        @click="onSubmit"
        class="rounded-lg px-4 py-2 text-sm font-medium border border-border bg-sidebar-accent hover:opacity-90"
      >
        Poslat na server
      </button>
    </div>
  </div>
</template>