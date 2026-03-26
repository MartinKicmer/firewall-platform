<script setup lang="ts">
import { ref } from 'vue'

type SubmitEvent = {
  action: number
  payload: {
    ID: number
    layer: 'L4TCP'
    save: boolean
    sourcePort: number
    destPort: number
    flags: number
    minWin: number
    maxWin: number
    limitCount: number
  }
}

const emit = defineEmits<{
  submit: [event: SubmitEvent]
}>()

const form = ref({
  ID: 1,
  permit: false,
  sourcePort: 444,
  destPort: -1,
  flags: 0,
  minWin: -1,
  maxWin: -1,
  limitCount: 1,
  save: false,
})

const onSubmit = () => {
  emit('submit', {
    action: form.value.permit ? 1 : 0,
    payload: {
      ID: form.value.ID,
      layer: 'L4TCP',
      save: form.value.save,
      sourcePort: form.value.sourcePort,
      destPort: form.value.destPort,
      flags: form.value.flags,
      minWin: form.value.minWin,
      maxWin: form.value.maxWin,
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
        <label class="block text-sm mb-1">Source port</label>
        <input
          v-model.number="form.sourcePort"
          type="number"
          class="w-full rounded-lg border border-border bg-background px-3 py-2"
        />
      </div>

      <div>
        <label class="block text-sm mb-1">Destination port</label>
        <input
          v-model.number="form.destPort"
          type="number"
          class="w-full rounded-lg border border-border bg-background px-3 py-2"
        />
      </div>

      <div>
        <label class="block text-sm mb-1">Flags</label>
        <input
          v-model.number="form.flags"
          type="number"
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

      <div>
        <label class="block text-sm mb-1">Min window size</label>
        <input
          v-model.number="form.minWin"
          type="number"
          class="w-full rounded-lg border border-border bg-background px-3 py-2"
        />
      </div>

      <div>
        <label class="block text-sm mb-1">Max window size</label>
        <input
          v-model.number="form.maxWin"
          type="number"
          class="w-full rounded-lg border border-border bg-background px-3 py-2"
        />
      </div>
    </div>

    <div class="mt-4 flex flex-wrap items-center gap-4">
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