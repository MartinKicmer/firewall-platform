<script setup lang="ts">
import type { FirewallRule, RuleLayer } from '@/types/firewall'

const props = defineProps<{
  rules: FirewallRule[]
  selectedLayer: RuleLayer
}>()

const emit = defineEmits<{
  deleteRule: [id: number]
}>()

const formatAction = (rule: FirewallRule) => {
  const permit = (rule as any)?.data?.permit
  return permit === true ? 'ALLOW' : 'DENY'
}
</script>

<template>
  <div class="overflow-x-auto rounded-xl border border-border bg-card">
    <table class="w-full text-sm">
      <thead class="bg-muted/40 border-b border-border">
        <tr>
          <th class="text-left px-4 py-3">ID</th>
          <th class="text-left px-4 py-3">Typ pravidla</th>
          <th class="text-left px-4 py-3">Akce</th>
          <th class="text-left px-4 py-3">Akce v tabulce</th>
        </tr>
      </thead>

      <tbody>
        <tr
          v-for="rule in props.rules"
          :key="rule.ID"
          class="border-b border-border last:border-b-0"
        >
          <td class="px-4 py-3">{{ rule.ID }}</td>
          <td class="px-4 py-3">{{ rule.ruleType }}</td>
          <td class="px-4 py-3">{{ formatAction(rule) }}</td>
          <td class="px-4 py-3">
            <button
              @click="emit('deleteRule', rule.ID)"
              class="rounded-lg px-3 py-1 text-sm border border-red-500/20 text-red-500 hover:bg-red-500/10"
            >
              Smazat
            </button>
          </td>
        </tr>

        <tr v-if="props.rules.length === 0">
          <td colspan="4" class="px-4 py-6 text-center text-muted-foreground">
            Žádná pravidla nebyla nalezena.
          </td>
        </tr>
      </tbody>
    </table>
  </div>
</template>