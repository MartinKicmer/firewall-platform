<script setup lang="ts">
import { ref } from 'vue'

type SubmitEvent = {
  action: number
  payload: {
    ID: number
    layer: 'L3'
    save: boolean
    source: [string, number]
    dest: [string, number]
    ttlMin: number
    ttlMax: number
    protocol: string
    tos: number
    allowFrag: boolean
    limitCount: number
  }
}

const emit = defineEmits<{
  submit: [event: SubmitEvent]
}>()

const form = ref({
  ID: 1,
  permit: false,
  sourceIp: '',
  sourceMask: 24,
  destIp: '',
  destMask: 24,
  ttlMin: -1,
  ttlMax: -1,
  protocol: 'TCP',
  tos: -1,
  allowFrag: true,
  limitCount: 1,
  save: false,
})

const onSubmit = () => {
  emit('submit', {
    action: form.value.permit ? 1 : 0,
    payload: {
      ID: form.value.ID,
      layer: 'L3',
      save: form.value.save,
      source: [form.value.sourceIp, form.value.sourceMask],
      dest: [form.value.destIp, form.value.destMask],
      ttlMin: form.value.ttlMin,
      ttlMax: form.value.ttlMax,
      protocol: form.value.protocol,
      tos: form.value.tos,
      allowFrag: form.value.allowFrag,
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
        <label class="block text-sm mb-1">Source IP</label>
        <input
          v-model="form.sourceIp"
          type="text"
          class="w-full rounded-lg border border-border bg-background px-3 py-2"
        />
      </div>

      <div>
        <label class="block text-sm mb-1">Source mask</label>
        <input
          v-model.number="form.sourceMask"
          type="number"
          class="w-full rounded-lg border border-border bg-background px-3 py-2"
        />
      </div>

      <div>
        <label class="block text-sm mb-1">Destination IP</label>
        <input
          v-model="form.destIp"
          type="text"
          class="w-full rounded-lg border border-border bg-background px-3 py-2"
        />
      </div>

      <div>
        <label class="block text-sm mb-1">Destination mask</label>
        <input
          v-model.number="form.destMask"
          type="number"
          class="w-full rounded-lg border border-border bg-background px-3 py-2"
        />
      </div>

      <div>
        <label class="block text-sm mb-1">Protocol</label>
        <select
          v-model="form.protocol"
          class="w-full rounded-lg border border-border bg-background px-3 py-2"
        >
          <option value="TCP">TCP</option>
          <option value="UDP">UDP</option>
          <option value="ICMP">ICMP</option>
        </select>
      </div>

      <div>
        <label class="block text-sm mb-1">TOS</label>
        <input
          v-model.number="form.tos"
          type="number"
          class="w-full rounded-lg border border-border bg-background px-3 py-2"
        />
      </div>

      <div>
        <label class="block text-sm mb-1">TTL min</label>
        <input
          v-model.number="form.ttlMin"
          type="number"
          class="w-full rounded-lg border border-border bg-background px-3 py-2"
        />
      </div>

      <div>
        <label class="block text-sm mb-1">TTL max</label>
        <input
          v-model.number="form.ttlMax"
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
    </div>

    <div class="mt-4 flex items-center gap-4">
      <label class="flex items-center gap-2 text-sm">
        <input v-model="form.allowFrag" type="checkbox" />
        <span>Povolit fragmenty</span>
      </label>

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