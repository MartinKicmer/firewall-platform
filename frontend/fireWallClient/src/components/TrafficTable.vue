<script setup lang="ts">
import type { TrafficRow } from '@/types/traffic'

const props = defineProps<{
  rows: TrafficRow[]
}>()
</script>

<template>
  <div class="overflow-x-auto rounded-xl border border-border bg-card">
    <table class="w-full text-sm">
      <thead class="bg-muted/40 border-b border-border">
        <tr>
          <th class="text-left px-4 py-3">Čas</th>
          <th class="text-left px-4 py-3">Vrstva</th>
          <th class="text-left px-4 py-3">Akce</th>
          <th class="text-left px-4 py-3">Zdroj</th>
          <th class="text-left px-4 py-3">Cíl</th>
          <th class="text-left px-4 py-3">Src Port</th>
          <th class="text-left px-4 py-3">Dst Port</th>
          <th class="text-left px-4 py-3">Protokol</th>
          <th class="text-left px-4 py-3">Info</th>
        </tr>
      </thead>

      <tbody>
        <tr
          v-for="(row, index) in props.rows"
          :key="index"
          class="border-b border-border last:border-b-0"
        >
          <td class="px-4 py-3">{{ row.timestamp ?? '-' }}</td>
          <td class="px-4 py-3">{{ row.layer ?? '-' }}</td>
          <td class="px-4 py-3">{{ row.action ?? '-' }}</td>
          <td class="px-4 py-3">{{ row.source ?? '-' }}</td>
          <td class="px-4 py-3">{{ row.destination ?? '-' }}</td>
          <td class="px-4 py-3">{{ row.sourcePort ?? '-' }}</td>
          <td class="px-4 py-3">{{ row.destinationPort ?? '-' }}</td>
          <td class="px-4 py-3">{{ row.protocol ?? '-' }}</td>
          <td class="px-4 py-3 whitespace-pre-wrap">{{ row.info ?? '-' }}</td>
        </tr>

        <tr v-if="props.rows.length === 0">
          <td colspan="9" class="px-4 py-6 text-center text-muted-foreground">
            Zatím nejsou žádná data provozu.
          </td>
        </tr>
      </tbody>
    </table>
  </div>
</template>